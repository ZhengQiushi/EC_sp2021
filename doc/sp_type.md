# sp_type

#### 包含文件

​	[sp_type.h](../SPLib/sp_type.h)

#### 主要内容

 1. 定义了常用的packed,union等关键字的简写

    ```c
    /* __packed keyword used to decrease the data type alignment to 1-byte */
    #if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 1)
        #define packed_struct       struct __attribute__((packed))
        #define packed_union        union __attribute__((packed))
    			
    
    		#define packed_struct_n(x)	struct __attribute__((packed, aligned(x)))
    		#define packed_union_n(x)	  union __attribute__((packed, aligned(x)))
    
    #else
        #define packed_struct       __packed struct
        #define packed_union        __packed union
    		
    
    		#define packed_struct_n(x)	__packed(x) struct
    		#define packed_union_n(x)	  __packed(x) union
    
    #endif
        
    #define spINLINE __inline
    #ifndef spFORCE_INLINE
        // #define portFORCE_INLINE __forceinline
        #define spFORCE_INLINE __attribute__((always_inline))
    #endif
    ```

    这些关键字的作用不明

	2. 特殊union联合体spHalfWord_t (16位)，spWord_t (32位)，spByte_t(8位)

    union型，包含完整数据uintX_t data(x = 8,16,32);和各位数据的打包结构体

    ```c
    packed_struct {uint8_t bit0:1;......} bits;
    ```
    可以读取数据的完整data和各位值bits.bitx  (0或1)

	3. 时间戳结构体

    ```c
    typedef struct {
        uint32_t        ms;                     /*!< Current millisecond */
        uint32_t        us;                     /*!< Current microsecond */
    } spTimeStamp;
    ```

    