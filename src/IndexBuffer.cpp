#include "IndexBuffer.h"
IndexBuffer::IndexBuffer() {
	mType = kXBufferObjectTypeIndexBuffer;
	mIndexes = nullptr;
	mIndexCount = 0;
	mCurrentIndex = 0;
}
IndexBuffer::~IndexBuffer() {
	if (mIndexCount != 0) {
		delete[] mIndexes;
	}
}
int IndexBuffer::GetSize() {
	return mIndexCount * sizeof(unsigned int);
}
void IndexBuffer::SetSize(int count) {
	mIndexCount = count;
	mIndexes = new unsigned int[count];
	XBufferObject::OnSetSize();
}
void IndexBuffer::SubmitData() {
	XBufferObject::SubmitData(mIndexes, GetSize());
}
void IndexBuffer::AppendIndex(int index) {
	mIndexes[mCurrentIndex++] = index;
}