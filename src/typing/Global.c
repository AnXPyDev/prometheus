Type TYPE_VOIDPTR;
Type TYPE_BYTEPTR;
Type TYPE_BYTESPAN;

void g_Typing_setupTypes(Allocator alc) {
	TYPE_VOIDPTR = Type_constcast(PointerType_create(PrimitiveType_upcast(PRIMITIVE_TYPE_VOID), alc));
	TYPE_BYTEPTR = Type_constcast(PointerType_create(PrimitiveType_upcast(PRIMITIVE_TYPE_BYTE), alc));
	TYPE_BYTESPAN = Type_constcast(SpanType_create(PrimitiveType_upcast(PRIMITIVE_TYPE_BYTE), alc));
}
