const int INT_1 = 1;
const int INT_0 = 0;

void g_SimValue_setup_consts(void) {
    SimValue_INT_0 = (SimValue) {
        .type = PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
        .data = (void*)&INT_0
    };

    SimValue_INT_1 = (SimValue) {
        .type = PrimitiveType_upcast(PRIMITIVE_TYPE_INT),
        .data = (void*)&INT_1
    };
}
