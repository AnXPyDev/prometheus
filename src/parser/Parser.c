typedef struct {
	Allocator allocator;
} Parser;

void Parser_create(Parser *this, Allocator allocator) {
	this->allocator = allocator;
}
