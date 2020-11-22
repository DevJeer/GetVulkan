#include "VertexBuffer.h"
VertexBuffer::VertexBuffer() {
	mType = kXBufferObjectTypeVertexBuffer;
	mVertexes = nullptr;
	mVertexCount = 0;
}

VertexBuffer::~VertexBuffer() {
	if (mVertexCount != 0) {
		delete[] mVertexes;
	}
}

int VertexBuffer::GetSize() {
	return mVertexCount * sizeof(XVertexData);
}

void VertexBuffer::SetSize(int count) {
	mVertexCount = count;
	mVertexes = new XVertexData[count];
	// ¿ª±Ù¿Õ¼ä
	XBufferObject::OnSetSize();
}

void VertexBuffer::SubmitData() {
	XBufferObject::SubmitData(mVertexes, GetSize());
}

void VertexBuffer::SetPosition(int index, float x, float y, float z, float w /* = 1.0f */) {
	mVertexes[index].SetPosition(x, y, z, w);
}

void VertexBuffer::SetTexcoord(int index, float x, float y, float z, float w /* = 1.0f */) {
	mVertexes[index].SetTexcoord(x, y, z, w);
}

void VertexBuffer::SetNormal(int index, float x, float y, float z, float w /* = 1.0f */) {
	mVertexes[index].SetNormal(x, y, z, w);
}

void VertexBuffer::SetTangent(int index, float x, float y, float z, float w /* = 1.0f */) {
	mVertexes[index].SetTangent(x, y, z, w);
}