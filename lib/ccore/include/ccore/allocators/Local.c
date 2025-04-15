#ifndef CCORE_LOCAL_ALLOCATOR_ALIGNMENT
#define CCORE_LOCAL_ALLOCATOR_ALIGNMENT 8
#endif 

#define MEMALIGN(x) memalign((x), CCORE_LOCAL_ALLOCATOR_ALIGNMENT)

typedef struct {
	char *buffer;
	char *buffer_end;
	char *bp;
	Allocator provider;
} LocalAllocator;

typedef struct {
	Size size;
} LocalAllocator_BufHeader;

#define HEADER_SIZE MEMALIGN(sizeof(LocalAllocator_BufHeader))
#define BUFFER_HEADER(mem) ((LocalAllocator_BufHeader*)((char*)mem - HEADER_SIZE));

void LocalAllocator_create(LocalAllocator *this, char *buffer, Size buffer_size, Allocator provider) {
	this->buffer = buffer;
	this->buffer_end = buffer + buffer_size;
	this->bp = buffer;
	this->provider = provider;
}

bool LocalAllocator_isLocal(LocalAllocator *this, void *mem) {
	return this->buffer <= (char*)mem && (char*)mem < this->buffer_end;
}

#define this ((LocalAllocator*)vthis)

void *LocalAllocator_malloc(void *vthis, Size size) {
	const Size bsize = MEMALIGN(size + HEADER_SIZE);
	if (this->bp + bsize > this->buffer_end) {
		return Allocator_malloc(this->provider, size);
	}
	char *buf = this->bp;
	LocalAllocator_BufHeader *mem = (LocalAllocator_BufHeader*)buf;
	mem->size = bsize;
	this->bp += bsize;
	return buf + HEADER_SIZE;
}

void *LocalAllocator_calloc(void *vthis, Size size) {
	const Size bsize = MEMALIGN(size + HEADER_SIZE);
	if (this->bp + bsize > this->buffer_end) {
		return Allocator_malloc(this->provider, size);
	}
	char *buf = this->bp;
	memset(buf, 0, bsize);

	LocalAllocator_BufHeader *mem = (LocalAllocator_BufHeader*)buf;
	mem->size = bsize;
	this->bp += bsize;
	return buf + HEADER_SIZE;
}

void *LocalAllocator_realloc(void *vthis, void *mem, Size size) {
	if (!LocalAllocator_isLocal(this, mem)) {
		return Allocator_realloc(this->provider, mem, size);
	}

	LocalAllocator_BufHeader *buf = BUFFER_HEADER(mem);
	if (size + HEADER_SIZE <= buf->size) {
		buf->size = size + HEADER_SIZE;
		return mem;
	}

	void *newmem = LocalAllocator_malloc(vthis, size);
	if (!newmem) return NULL;

	memcpy(newmem, mem, buf->size - HEADER_SIZE);

	return newmem;
}

void LocalAllocator_free(void *vthis, void *mem) {
	if (this->buffer <= (char*)mem && (char*)mem < this->buffer_end) {
		return;
	}
	Allocator_free(this->provider, mem);
}

void LocalAllocator_destroy(void *vthis) {}

#undef this

void LocalAllocator_clear(LocalAllocator *this) {
	this->bp = this->buffer;
}

const IAllocator IAllocator_LocalAllocator = {
	.malloc = &LocalAllocator_malloc,
	.calloc = &LocalAllocator_calloc,
	.free = &LocalAllocator_free,
	.realloc = &LocalAllocator_realloc,
	.destroy = &LocalAllocator_destroy
};

Allocator LocalAllocator_upcast(LocalAllocator *this) {
	return (Allocator) { .interface = &IAllocator_LocalAllocator, .object = this };
}

#undef BUFFER_HEADER
#undef HEADER_SIZE
#undef MEMALIGN
