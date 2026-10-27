/**
 * @file RF24CCM.h
 *
 * An RF24 radio wrapper that encrypts and authenticates every radio frame with AES-128 CCM,
 * using the same over-the-air format as the nrf_to_nrf library's CCM encryption.
 * This lets Linux nodes (RF24Network, RF24Mesh and RF24Gateway) communicate with
 * nRF52840 nodes that have `radio.enableEncryption = true`.
 *
 * Frame format (the dynamic payload as seen by an nRF24L01):
 *
 * | IV (5 bytes) | packet counter (3 bytes, little endian) | ciphertext | MIC (4 bytes) |
 *
 * The nonce follows the Bluetooth LE CCM layout used by the nRF52 CCM peripheral:
 * packet counter (5 bytes, little endian, direction bit 0) followed by the 8 byte IV
 * (5 transmitted bytes, 3 zero bytes). The additional authenticated data is a single 0x00 byte.
 *
 * Every frame carries 12 bytes of overhead, so an encrypted 32 byte frame holds 20 bytes of
 * RF24Network data: an 8 byte header plus 12 bytes of payload.
 */
#ifndef __RF24CCM_H__
#define __RF24CCM_H__

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/random.h>
#if !defined(USE_RF24_LIB_SRC)
    #include <RF24/RF24.h>
#else
    #include <RF24.h>
#endif

#define RF24CCM_KEY_SIZE     16
#define RF24CCM_IV_SIZE      5 // must match CCM_IV_SIZE in nrf_to_nrf.h
#define RF24CCM_COUNTER_SIZE 3 // must match CCM_COUNTER_SIZE in nrf_to_nrf.h
#define RF24CCM_MIC_SIZE     4 // must match CCM_MIC_SIZE in nrf_to_nrf.h
#define RF24CCM_NONCE_SIZE   13
#define RF24CCM_OVERHEAD     (RF24CCM_IV_SIZE + RF24CCM_COUNTER_SIZE + RF24CCM_MIC_SIZE)
#define RF24CCM_FRAME_SIZE   32 // max dynamic payload size of the nRF24L01

namespace rf24ccm {

inline uint8_t xtime(uint8_t x)
{
    return (uint8_t)((x << 1) ^ ((x & 0x80) ? 0x1B : 0x00));
}

inline uint8_t rotl8(uint8_t x, uint8_t shift)
{
    return (uint8_t)((x << shift) | (x >> (8 - shift)));
}

// The AES S-box, generated once instead of typed out as a table
inline const uint8_t* sbox()
{
    static uint8_t box[256];
    static bool ready = false;
    if (!ready) {
        uint8_t p = 1, q = 1;
        do {
            p = (uint8_t)(p ^ (p << 1) ^ ((p & 0x80) ? 0x1B : 0x00)); // multiply p by 3
            q ^= (uint8_t)(q << 1);                                   // divide q by 3
            q ^= (uint8_t)(q << 2);
            q ^= (uint8_t)(q << 4);
            if (q & 0x80) {
                q ^= 0x09;
            }
            box[p] = (uint8_t)(q ^ rotl8(q, 1) ^ rotl8(q, 2) ^ rotl8(q, 3) ^ rotl8(q, 4) ^ 0x63);
        } while (p != 1);
        box[0] = 0x63;
        ready = true;
    }
    return box;
}

inline void aesExpandKey(const uint8_t key[16], uint8_t roundKeys[176])
{
    const uint8_t* sb = sbox();
    uint8_t rcon = 0x01;
    memcpy(roundKeys, key, 16);
    for (uint8_t i = 16; i < 176; i += 4) {
        uint8_t t[4] = {roundKeys[i - 4], roundKeys[i - 3], roundKeys[i - 2], roundKeys[i - 1]};
        if (i % 16 == 0) {
            uint8_t first = t[0];
            t[0] = (uint8_t)(sb[t[1]] ^ rcon);
            t[1] = sb[t[2]];
            t[2] = sb[t[3]];
            t[3] = sb[first];
            rcon = xtime(rcon);
        }
        for (uint8_t j = 0; j < 4; j++) {
            roundKeys[i + j] = (uint8_t)(roundKeys[i - 16 + j] ^ t[j]);
        }
    }
}

inline void aesEncryptBlock(const uint8_t roundKeys[176], const uint8_t in[16], uint8_t out[16])
{
    const uint8_t* sb = sbox();
    uint8_t s[16];
    for (uint8_t i = 0; i < 16; i++) {
        s[i] = (uint8_t)(in[i] ^ roundKeys[i]);
    }
    for (uint8_t round = 1; round <= 10; round++) {
        for (uint8_t i = 0; i < 16; i++) {
            s[i] = sb[s[i]];
        }
        // ShiftRows: row r is rotated left by r bytes
        uint8_t t = s[1];
        s[1] = s[5];
        s[5] = s[9];
        s[9] = s[13];
        s[13] = t;
        t = s[2];
        s[2] = s[10];
        s[10] = t;
        t = s[6];
        s[6] = s[14];
        s[14] = t;
        t = s[15];
        s[15] = s[11];
        s[11] = s[7];
        s[7] = s[3];
        s[3] = t;
        if (round < 10) {
            for (uint8_t c = 0; c < 16; c += 4) {
                uint8_t a0 = s[c], a1 = s[c + 1], a2 = s[c + 2], a3 = s[c + 3];
                uint8_t all = (uint8_t)(a0 ^ a1 ^ a2 ^ a3);
                s[c] = (uint8_t)(a0 ^ all ^ xtime((uint8_t)(a0 ^ a1)));
                s[c + 1] = (uint8_t)(a1 ^ all ^ xtime((uint8_t)(a1 ^ a2)));
                s[c + 2] = (uint8_t)(a2 ^ all ^ xtime((uint8_t)(a2 ^ a3)));
                s[c + 3] = (uint8_t)(a3 ^ all ^ xtime((uint8_t)(a3 ^ a0)));
            }
        }
        for (uint8_t i = 0; i < 16; i++) {
            s[i] ^= roundKeys[round * 16 + i];
        }
    }
    memcpy(out, s, 16);
}

// CBC-MAC part of CCM (RFC 3610 / NIST SP 800-38C). nonceLen is 7..13, the AAD must be < 0xFF00 bytes
inline void ccmMac(const uint8_t roundKeys[176], const uint8_t* nonce, uint8_t nonceLen, const uint8_t* aad, size_t aadLen,
                   const uint8_t* msg, size_t msgLen, uint8_t micLen, uint8_t mac[16])
{
    const uint8_t L = (uint8_t)(15 - nonceLen);
    uint8_t block[16] = {0};
    block[0] = (uint8_t)((aadLen ? 0x40 : 0x00) | (((micLen - 2) / 2) << 3) | (L - 1));
    memcpy(&block[1], nonce, nonceLen);
    for (uint8_t i = 0; i < L; i++) {
        block[15 - i] = (uint8_t)(i < sizeof(size_t) ? (msgLen >> (8 * i)) : 0);
    }
    aesEncryptBlock(roundKeys, block, mac);

    if (aadLen) {
        memset(block, 0, sizeof(block));
        block[0] = (uint8_t)(aadLen >> 8);
        block[1] = (uint8_t)aadLen;
        uint8_t pos = 2;
        for (size_t i = 0; i < aadLen; i++) {
            block[pos++] = aad[i];
            if (pos == 16) {
                for (uint8_t j = 0; j < 16; j++) {
                    mac[j] ^= block[j];
                }
                aesEncryptBlock(roundKeys, mac, mac);
                memset(block, 0, sizeof(block));
                pos = 0;
            }
        }
        if (pos) {
            for (uint8_t j = 0; j < 16; j++) {
                mac[j] ^= block[j];
            }
            aesEncryptBlock(roundKeys, mac, mac);
        }
    }

    for (size_t i = 0; i < msgLen; i += 16) {
        size_t n = (msgLen - i < 16) ? msgLen - i : 16;
        for (size_t j = 0; j < n; j++) {
            mac[j] ^= msg[i + j];
        }
        aesEncryptBlock(roundKeys, mac, mac);
    }
}

// CTR part of CCM: XORs `len` bytes with key stream blocks S_1, S_2, ... and returns S_0 in s0
inline void ccmCtr(const uint8_t roundKeys[176], const uint8_t* nonce, uint8_t nonceLen, const uint8_t* in, size_t len,
                   uint8_t* out, uint8_t s0[16])
{
    const uint8_t L = (uint8_t)(15 - nonceLen);
    uint8_t ctr[16] = {0};
    uint8_t stream[16];
    ctr[0] = (uint8_t)(L - 1);
    memcpy(&ctr[1], nonce, nonceLen);
    aesEncryptBlock(roundKeys, ctr, s0);

    uint32_t counter = 1;
    for (size_t i = 0; i < len; i += 16, counter++) {
        for (uint8_t j = 0; j < L; j++) {
            ctr[15 - j] = (uint8_t)(j < 4 ? (counter >> (8 * j)) : 0);
        }
        aesEncryptBlock(roundKeys, ctr, stream);
        size_t n = (len - i < 16) ? len - i : 16;
        for (size_t j = 0; j < n; j++) {
            out[i + j] = (uint8_t)(in[i + j] ^ stream[j]);
        }
    }
}

/** Encrypt `len` bytes of `in` into `out` (may be the same buffer) and write a `micLen` byte tag to `mic` */
inline void ccmEncrypt(const uint8_t roundKeys[176], const uint8_t* nonce, uint8_t nonceLen, const uint8_t* aad, size_t aadLen,
                       const uint8_t* in, size_t len, uint8_t* out, uint8_t* mic, uint8_t micLen)
{
    uint8_t mac[16], s0[16];
    ccmMac(roundKeys, nonce, nonceLen, aad, aadLen, in, len, micLen, mac);
    ccmCtr(roundKeys, nonce, nonceLen, in, len, out, s0);
    for (uint8_t i = 0; i < micLen; i++) {
        mic[i] = (uint8_t)(mac[i] ^ s0[i]);
    }
}

/** Decrypt `len` bytes of `in` into `out` and verify the tag. On failure `out` is zeroed and false is returned */
inline bool ccmDecrypt(const uint8_t roundKeys[176], const uint8_t* nonce, uint8_t nonceLen, const uint8_t* aad, size_t aadLen,
                       const uint8_t* in, size_t len, const uint8_t* mic, uint8_t micLen, uint8_t* out)
{
    uint8_t mac[16], s0[16];
    ccmCtr(roundKeys, nonce, nonceLen, in, len, out, s0);
    ccmMac(roundKeys, nonce, nonceLen, aad, aadLen, out, len, micLen, mac);
    uint8_t diff = 0;
    for (uint8_t i = 0; i < micLen; i++) {
        diff |= (uint8_t)(mac[i] ^ s0[i] ^ mic[i]);
    }
    if (diff) {
        memset(out, 0, len);
        return false;
    }
    return true;
}

} // namespace rf24ccm

/**
 * Drop-in replacement for RF24 that encrypts outgoing frames and decrypts incoming ones.
 *
 * Use it as the `radio_t` of ESBNetwork, ESBMesh and ESBGateway. The libraries must be built
 * with the `RF24NETWORK_CCM` option so these template instantiations exist.
 *
 * When encryption is enabled, frames that are too short or fail the MIC check are dropped,
 * counted in decryptFailures, and reported to RF24Network as a zero length payload
 * (which RF24Network::update() returns as NETWORK_CORRUPTION).
 */
class RF24CCM : public RF24
{
public:
    using RF24::RF24;

    /** Encrypt and authenticate all traffic. Every node on the channel must use the same key */
    bool enableEncryption = false;

    /** Received frames dropped because they were too short or failed authentication */
    uint32_t decryptFailures = 0;

    /** Set the 16 byte (128 bit) key. Use the same byte order as nrf_to_nrf::setKey() */
    void setKey(const uint8_t key[RF24CCM_KEY_SIZE])
    {
        rf24ccm::aesExpandKey(key, roundKeys);
        keySet = true;
    }

    /** Set the packet counter for outgoing frames. It starts at a random value and wraps at 24 bits */
    void setCounter(uint32_t counter)
    {
        packetCounter = counter;
    }

    /** RF24::setRetries(), raised to at least minRetryDelay and minRetryCount while encryption is enabled */
    void setRetries(uint8_t delay, uint8_t count)
    {
        if (enableEncryption) {
            delay = delay > minRetryDelay ? delay : minRetryDelay;
            count = count > minRetryCount ? count : minRetryCount;
        }
        RF24::setRetries(delay, count);
    }

    uint8_t getDynamicPayloadSize(void)
    {
        if (!enableEncryption) {
            return RF24::getDynamicPayloadSize();
        }
        if (rxPending) {
            return rxLength;
        }
        uint8_t size = RF24::getDynamicPayloadSize(); // 0 means RF24 flushed a corrupt payload
        if (!size) {
            return 0;
        }
        uint8_t frame[RF24CCM_FRAME_SIZE];
        RF24::read(frame, size);
        if (!decryptFrame(frame, size)) {
            ++decryptFailures;
            return 0;
        }
        rxPending = true;
        return rxLength;
    }

    void read(void* buf, uint8_t len)
    {
        if (!enableEncryption) {
            RF24::read(buf, len);
            return;
        }
        if (!rxPending && !getDynamicPayloadSize()) {
            memset(buf, 0, len);
            return;
        }
        memcpy(buf, rxPlain, len < rxLength ? len : rxLength);
        rxPending = false;
    }

    bool write(const void* buf, uint8_t len)
    {
        return write(buf, len, false);
    }

    bool write(const void* buf, uint8_t len, const bool multicast)
    {
        if (!enableEncryption) {
            return RF24::write(buf, len, multicast);
        }
        uint8_t frame[RF24CCM_FRAME_SIZE];
        if (!encryptFrame(buf, len, frame)) {
            return false;
        }
        return RF24::write(frame, (uint8_t)(len + RF24CCM_OVERHEAD), multicast);
    }

    bool writeFast(const void* buf, uint8_t len)
    {
        return writeFast(buf, len, false);
    }

    bool writeFast(const void* buf, uint8_t len, const bool multicast)
    {
        if (!enableEncryption) {
            return RF24::writeFast(buf, len, multicast);
        }
        uint8_t frame[RF24CCM_FRAME_SIZE];
        if (!encryptFrame(buf, len, frame)) {
            return false;
        }
        return RF24::writeFast(frame, (uint8_t)(len + RF24CCM_OVERHEAD), multicast);
    }

private:
    uint8_t roundKeys[176] = {0};
    bool keySet = false;
    uint32_t packetCounter = randomCounter();
    uint8_t rxPlain[RF24CCM_FRAME_SIZE] = {0};
    uint8_t rxLength = 0;
    bool rxPending = false;

    static uint32_t randomCounter()
    {
        uint32_t value = 0;
        if (getrandom(&value, sizeof(value), 0) != sizeof(value)) {
            value = 0;
        }
        return value & 0xFFFFFF;
    }

    static void buildNonce(const uint8_t* iv, const uint8_t* counter, uint8_t nonce[RF24CCM_NONCE_SIZE])
    {
        // Bytes 0-4: 39 bit packet counter + direction bit (always 0 in nrf_to_nrf), bytes 5-12: IV
        memset(nonce, 0, RF24CCM_NONCE_SIZE);
        memcpy(nonce, counter, RF24CCM_COUNTER_SIZE);
        memcpy(&nonce[5], iv, RF24CCM_IV_SIZE);
    }

    bool encryptFrame(const void* buf, uint8_t len, uint8_t frame[RF24CCM_FRAME_SIZE])
    {
        if (!keySet || !len || len > RF24CCM_FRAME_SIZE - RF24CCM_OVERHEAD) {
            return false;
        }
        // Never reuse a nonce: refuse to send if the random IV can't be generated
        if (getrandom(frame, RF24CCM_IV_SIZE, 0) != RF24CCM_IV_SIZE) {
            return false;
        }
        uint32_t counter = packetCounter;
        packetCounter = (packetCounter + 1) & 0xFFFFFF;
        uint8_t* counterBytes = &frame[RF24CCM_IV_SIZE];
        for (uint8_t i = 0; i < RF24CCM_COUNTER_SIZE; i++) {
            counterBytes[i] = (uint8_t)(counter >> (8 * i));
        }

        uint8_t nonce[RF24CCM_NONCE_SIZE];
        buildNonce(frame, counterBytes, nonce);
        const uint8_t aad = 0x00;
        uint8_t* cipher = &frame[RF24CCM_IV_SIZE + RF24CCM_COUNTER_SIZE];
        rf24ccm::ccmEncrypt(roundKeys, nonce, RF24CCM_NONCE_SIZE, &aad, 1, static_cast<const uint8_t*>(buf), len, cipher,
                            &cipher[len], RF24CCM_MIC_SIZE);
        return true;
    }

    bool decryptFrame(const uint8_t* frame, uint8_t size)
    {
        if (!keySet || size <= RF24CCM_OVERHEAD || size > RF24CCM_FRAME_SIZE) {
            return false;
        }
        const uint8_t* iv = frame;
        const uint8_t* counter = &frame[RF24CCM_IV_SIZE];
        const uint8_t* cipher = &frame[RF24CCM_IV_SIZE + RF24CCM_COUNTER_SIZE];
        uint8_t cipherLength = (uint8_t)(size - RF24CCM_OVERHEAD);

        uint8_t nonce[RF24CCM_NONCE_SIZE];
        buildNonce(iv, counter, nonce);
        const uint8_t aad = 0x00;
        if (!rf24ccm::ccmDecrypt(roundKeys, nonce, RF24CCM_NONCE_SIZE, &aad, 1, cipher, cipherLength, &cipher[cipherLength],
                                 RF24CCM_MIC_SIZE, rxPlain)) {
            return false;
        }
        rxLength = cipherLength;
        return true;
    }

    // New data members go below this line, so existing members keep their offsets and a program or
    // library built against an older RF24CCM.h still finds the key and buffers where it expects them.
    /**
     * While encryption is enabled, setRetries() never goes below these values (delay in 250us steps).
     * Encrypted frames carry half the data, so messages take twice as many frames, and nrf_to_nrf only
     * ACKs a frame when its sketch polls the radio. RF24Network's 5 retries are then often not enough,
     * e.g. for the replies a node waits on while connecting to an MQTT broker. Set these and
     * enableEncryption before network/mesh/gateway begin(), which is where RF24Network sets the retries.
     */
    uint8_t minRetryDelay = 15;
    uint8_t minRetryCount = 15;
};

#endif // __RF24CCM_H__
