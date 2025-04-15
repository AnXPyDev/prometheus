typedef struct {
    FILE *file;
} FileOutStream;


#define this ((FileOutStream*)vthis)

void FileOutStream_putc(void *vthis, int c) {
    fputc(c, this->file);
}

void FileOutStream_puts(void *vthis, const char *s) {
    fputs(s, this->file);
}

void FileOutStream_write(void *vthis, BufferView buf) {
    fwrite(buf.data, 1, buf.size, this->file);
}

void FileOutStream_flush(void *vthis) {
    fflush(this->file);
}

void FileOutStream_close(void *vthis) {
    fclose(this->file);
}

#undef this


const IOutStream IFileOutStream = {
    .putc = &FileOutStream_putc,
    .puts = &FileOutStream_puts,
    .write = &FileOutStream_write,
    .flush = &FileOutStream_flush,
    .close = &FileOutStream_close
};

void FileOutStream_create(FileOutStream *this, FILE *fp) {
    this->file = fp;
}

FileOutStream FileOutStream_new(FILE *fp) {
    FileOutStream this;
    FileOutStream_create(&this, fp);
    return this;
}

OutStream FileOutStream_upcast(FileOutStream *this) {
    OutStream outstream = { &IFileOutStream, this };
    return outstream;
}
