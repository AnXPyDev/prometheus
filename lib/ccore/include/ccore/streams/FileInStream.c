typedef struct {
    FILE *file;
} FileInStream;

#define this ((FileInStream*)vthis)

int FileInStream_getc(void *vthis) {
    return fgetc(this->file);
}

Size FileInStream_read(void *vthis, Buffer buf) {
    return fread(buf.data, 1, buf.size, this->file);
}

bool FileInStream_end(void *vthis) {
    return feof(this->file) != 0;
}

void FileInStream_close(void *vthis) {
    fclose(this->file);
}

#undef this

const IInStream IFileInStream = {
    .getc = &FileInStream_getc,
    .read = &FileInStream_read,
    .end = &FileInStream_end,
    .close = &FileInStream_close
};

void FileInStream_create(FileInStream *this, FILE *fp) {
    this->file = fp;
}

FileInStream FileInStream_new(FILE *fp) {
    FileInStream this;
    FileInStream_create(&this, fp);
    return this;
}

InStream FileInStream_upcast(FileInStream *this) {
    return (InStream) {
        .interface = &IFileInStream,
        .object = (void*)this
    };
}
