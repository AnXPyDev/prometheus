typedef struct {
    void (*putc)(void *this, int c);
    void (*puts)(void *this, const char *s);
    void (*write)(void *this, BufferView buf);
    void (*flush)(void *this);
    void (*close)(void *this);
} IOutStream;

typedef struct {
    const IOutStream *interface;
    void *object;
} OutStream;

const OutStream OutStream_NULL = {
    .interface = NULL,
    .object = NULL
};

bool OutStream_isNull(OutStream this) {
   return this.interface == NULL; 
}

void OutStream_putc(OutStream this, int c) {
    this.interface->putc(this.object, c);
}

void OutStreamPtr_putc(void *this, int c) {
    OutStream_putc(*(OutStream*)this, c);
}


void OutStream_write(OutStream this, BufferView buf) {
    if (this.interface->write != NULL) {
        this.interface->write(this.object, buf);
        return;
    }

    for (Size i = 0; i < buf.size; i++) {
        OutStream_putc(this, (int)buf.data[i]);
    }
}

void OutStreamPtr_write(void *this, BufferView buf) {
    OutStream_write(*(OutStream*)this, buf);
}

void OutStream_puts(OutStream this, const char *s) {
    if (this.interface->puts != NULL) {
        this.interface->puts(this.object, s);
        return;
    }

    OutStream_write(this, strview(s));
}

void OutStreamPtr_puts(void *this, const char *s) {
    OutStream_puts(*(OutStream*)this, s);
}

void OutStream_writes(OutStream this, BufferView buf) {
    if (buf.data == NULL) {
        OutStream_puts(this, "(null)");
        return;
    }

    OutStream_write(this, buf);
}


void OutStream_flush(OutStream this) {
    if (this.interface->flush != NULL) {
        this.interface->flush(this.object);
        return;
    }
}

void OutStreamPtr_flush(void *this) {
    OutStream_flush(*(OutStream*)this);
}

void OutStream_close(OutStream this) {
    if (this.interface->close != NULL) {
        this.interface->close(this.object);
        return;
    }
}

void OutStreamPtr_close(void *this) {
    OutStream_close(*(OutStream*)this);
}
