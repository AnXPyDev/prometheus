typedef uint64_t Hash;
typedef uint32_t HashChunk;

#define CCORE_HASH_BASE 5381

const Hash Hash_NULL = 0;

// should be inlined
Hash Hash_combine(Hash this, Hash other) {
	return ((this << 5) + this) + other;
}

Hash Hash_combineArr(Hash *hashes, Size n) {
	Hash hash = *(hashes++);
	while (--n) {
		hash = Hash_combine(*(hashes++), n);
	}
	return hash;
}

Hash Hash_fromChunks(const HashChunk *chunks, Size size) {
	Hash hash = CCORE_HASH_BASE;
	while ( (size--) != 0 ) {
		Hash val = (Hash)*(chunks++);
		hash = Hash_combine(hash, val);
	}
	return hash;
}

Hash Hash_fromBytes_nochunk(const char *bytes, Size size) {
	Hash hash = CCORE_HASH_BASE;
	while ( (size--) != 0 ) {
		Hash val = (Hash)*(bytes++);
		hash = Hash_combine(hash, val);
	}
	return hash;
}

Hash Hash_fromBytes(const char *bytes, Size size) {
	const Size align = size % sizeof(HashChunk);
	const Size chunks = size / sizeof(HashChunk);

	Hash hash = Hash_fromChunks((HashChunk*)bytes, chunks);
	if (align == 0) return hash;

	return Hash_combine(hash, Hash_fromBytes_nochunk(bytes + (size - align), align));
}

Hash Hash_fromIntPtr(const intptr_t val) {
	// should be optimized away
	if (sizeof(intptr_t) == sizeof(Hash)) {
		return Hash_combine(CCORE_HASH_BASE, (Hash)val);
	} else if (sizeof(intptr_t) % sizeof(HashChunk) == 0) {
		return Hash_fromChunks((HashChunk*)&val, sizeof(void*) / sizeof(HashChunk));
	} else if (sizeof(intptr_t) > sizeof(HashChunk)) {
		return Hash_fromBytes_nochunk((const char*)&val, sizeof(void*));
	} else {
		return Hash_fromBytes((char*)&val, sizeof(void*));
	}
}

Hash Hash_fromPtr(const void *ptr) {
	return Hash_fromIntPtr((intptr_t)ptr);
}

Hash Hash_fromInt(int val) {
	if (sizeof(int) <= sizeof(Hash)) {
		return Hash_combine(CCORE_HASH_BASE, (Hash)val);
	} else {
		return Hash_fromBytes((char*)&val, sizeof(void*));
	}
}

Hash Hash_fromSize(Size val) {
	if (sizeof(Size) <= sizeof(Hash)) {
		return Hash_combine(CCORE_HASH_BASE, (Hash)val);
	} else {
		return Hash_fromBytes((char*)&val, sizeof(void*));
	}
}

Hash BufferView_hash(BufferView buf) {
	return Hash_fromBytes(buf.data, buf.size);
}
