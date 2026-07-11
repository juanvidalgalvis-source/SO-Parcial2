#include "Page.h"

Page::Page(int pageNumber)
    : pageNumber(pageNumber), frameNumber(-1), valid(false) {
}

void Page::assignFrame(int frameNumber) {
    this->frameNumber = frameNumber;
    this->valid = true;
}

void Page::invalidate() {
    this->frameNumber = -1;
    this->valid = false;
}

int Page::getPageNumber() const {
    return pageNumber;
}

int Page::getFrameNumber() const {
    return frameNumber;
}

bool Page::isValid() const {
    return valid;
}
