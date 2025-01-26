#ifndef ASSEMBLER_C_EXTERN_C_H
#define ASSEMBLER_C_EXTERN_C_H

#ifdef __cplusplus
    #define EXTERN_C_BEGIN extern "C" {
    #define EXTERN_C_END }
#else
    #define EXTERN_C_BEGIN
    #define EXTERN_C_END
#endif

#endif  // ASSEMBLER_C_EXTERN_C_H
