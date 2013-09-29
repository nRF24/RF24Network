inline void eeprom_update_block ( const void *__src, void * __dst, size_t __n)
{
  const uint8_t* __src_u = reinterpret_cast<const uint8_t*>(__src);
  uint8_t* __dst_u = reinterpret_cast<uint8_t*>(__dst);
  while (__n--)
  {
    if (eeprom_read_byte(__dst_u) != *__src_u)
      eeprom_write_byte(__dst_u,*__src_u);
    ++__src_u;
    ++__dst_u;
  }
}

// vim:cin:ai:sts=2 sw=2 ft=cpp
