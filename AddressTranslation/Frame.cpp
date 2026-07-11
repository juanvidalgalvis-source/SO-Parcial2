#include "Frame.h"

Frame::Frame(int frameNumber)
    : frameNumber(frameNumber), free(true), pid(-1), pageNumber(-1) {
}

void Frame::occupy(int pid, int pageNumber) {
    this->free = false;
    this->pid = pid;
    this->pageNumber = pageNumber;
}

void Frame::release() {
    this->free = true;
    this->pid = -1;
    this->pageNumber = -1;
}

int Frame::getFrameNumber() const {
    return frameNumber;
}

bool Frame::isFree() const {
    return free;
}

int Frame::getPid() const {
    return pid;
}

int Frame::getPageNumber() const {
    return pageNumber;
}
