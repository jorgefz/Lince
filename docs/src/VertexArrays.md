# Vertex Arrays

## LinceVertexArray
- `unsigned int id`
- `LinceIndexBuffer index_buffer`
- `LinceVertexBuffer vb_list`
- `unsigned int vb_count`


## LinceCreateVertexArray
```c
LinceVertexArray* LinceCreateVertexArray(LinceIndexBuffer index_buffer)
```
## LinceBindVertexArray
```c
void LinceBindVertexArray(LinceVertexArray* vertex_array)
```
## LinceUnbindVertexArray
```c
void LinceUnbindVertexArray(void)
```
## LinceAddVertexArrayAttributes
```c
void LinceAddVertexArrayAttributes(
	LinceVertexArray* vertex_array,
	LinceVertexBuffer vertex_buffer,
	LinceBufferElement* layout,
	unsigned int layout_elements
)
```
## LinceDeleteVertexArray
```c
void LinceDeleteVertexArray(LinceVertexArray* vertex_array)
```