# Buffers

Buffers are simply memory spaces where vertex data is passed onto OpenGL and stored.

## LinceBufferType

Describes the data type of elements in a buffer.
These are usually integers or floats, and arrays of these with fixed sizes, including square matrices.

| Enum                   | Value | Components | Bytes |
| ---------------------- | ----- | ---------- | ----- |
| LinceBufferType_None   |   0   | 		0	  |   0   |
| LinceBufferType_Bool   |   1   | 		1	  |   4   |
| LinceBufferType_Int    |   2   | 		1	  |   4   |
| LinceBufferType_Int2   |   3   | 		2	  |   8   |
| LinceBufferType_Int3   |   4   | 		3	  |   12  |
| LinceBufferType_Int4   |   5   | 		4	  |   16  |
| LinceBufferType_Float  |   6   | 		1	  |   4   |
| LinceBufferType_Float2 |   7   | 		2	  |   8   |
| LinceBufferType_Float3 |   8   | 		3	  |   12  |
| LinceBufferType_Float4 |   9   | 		4	  |   16  |
| LinceBufferType_Mat3   |   10  | 		9	  |   36  |
| LinceBufferType_Mat4   |   11  | 		16	  |   64  |
| LinceBufferType_Count  |   12  | 		--	  |   --  |

## LinceBufferElement

- `LinceBufferType type`
- `const char name[LINCE_NAME_MAX]`
- `unsigned int gl_type`
- `unsigned int comps`
- `unsigned int bytes`
- `unsigned int offset`


## LinceGetBufferTypeData
```c
LinceBufferElement LinceGetBufferTypeData(LinceBufferType type)
```

## LinceSetupBufferElementData
```c
void LinceSetupBufferElementData(LinceBufferElement* elem)
```


## LinceVertexBuffer
```c
typedef unsigned int LinceVertexBuffer
```
## LinceCreateVertexBuffer
```c
LinceVertexBuffer LinceCreateVertexBuffer(void* data, unsigned int size)
```
## LinceSetVertexBufferData
```c
void LinceSetVertexBufferData(LinceVertexBuffer vb, void* data, unsigned int size)
```
## LinceBindVertexBuffer
```c
void LinceBindVertexBuffer(LinceVertexBuffer vb)
```
## LinceUnbindVertexBuffer
```c
void LinceUnbindVertexBuffer()
```
## LinceDeleteVertexBuffer
```c
void LinceDeleteVertexBuffer(LinceVertexBuffer vb)
```


## LinceIndexBuffer

- `unsigned int id`
- `unsigned int count`

## LinceCreateIndexBuffer
```c
LinceIndexBuffer LinceCreateIndexBuffer(unsigned int* data, unsigned int count)
```
## LinceBindIndexBuffer
```c
void LinceBindIndexBuffer(LinceIndexBuffer ib)
```
## LinceUnbindIndexBuffer
```c
void LinceUnbindIndexBuffer()
```
## LinceDeleteIndexBuffer
```c
void LinceDeleteIndexBuffer(LinceIndexBuffer ib)
```
