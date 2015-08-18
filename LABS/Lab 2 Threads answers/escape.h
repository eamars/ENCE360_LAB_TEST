

// escape dangerous characters to stop misuse
inline void escape(char *out, int bufferSize, char *in) {
  
  char *end = out + bufferSize - 1;
  
  while(*in != '\0' && out != end) {

    // escape quote (")
    if(*in == '\"') {
      *out++ = '\\';
    }
    
    // escape backslash (\)
    if(*in == '\\') {
      *out++ = '\\';
    }    
    
    *out++ = *in++;    
  }
  
  *out = '\0';
}
