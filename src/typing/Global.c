Type TYPE_VOIDPTR;
Type TYPE_BYTESPAN;

void g_Typing_setupTypes(Allocator alc) {
	TYPE_VOIDPTR = PointerType_create(PrimitiveType_upcast(PRIMITIVE_TYPE_VOID), alc);
	TYPE_BYTESPAN = SpanType_create(PrimitiveType_upcast(PRIMITIVE_TYPE_BYTE), alc);
}
