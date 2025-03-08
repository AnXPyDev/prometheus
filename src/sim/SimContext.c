void SimState_init(SimState *this) {
	SimCache_create(&this->cache, this->temp_alc);
	this->root_frame = NULL;
}
