//#include "stdafx.h"
#include "PickPkgContent.h"
#include <QDebug>
#include <QFile>
#include <fstream>

PickPkgContent::PickPkgContent(void)
{
	m_count         = 0;
	m_buffer_width  = 0;
	m_buffer_height = 0;
	m_byteperpixel   = 0;
	m_skip_pkg      = 0;
	m_searchpoint_InData   = 0;
	m_pkg_vector = NULL;
	m_currentIndex = 0;
	m_currentPkgIndex = 0;
	m_PickpkgIndex = 0;
	m_lasterror = NO_ERROR;
	m_check_bytes = 2;
	m_debug_test = 0;
	test = 0;
}


PickPkgContent::~PickPkgContent(void)
{
	DestroyBuffer();
}
void PickPkgContent::RegisterType(int type){
	m_type = type;
}

void PickPkgContent::SetBufferSize(int x,int y){
	 m_buffer_width  = x;
	 m_buffer_height = y;
}

void PickPkgContent::SetBufferCount(int count){
	m_count = count;
}

int  PickPkgContent::GetBufferIndex(){
	return m_currentIndex;
}

BOOL PickPkgContent::CreateBuffer(){
	BYTE* tmp;
	if((m_count == 0) || (m_buffer_width == 0) || (m_buffer_height == 0) || (m_byteperpixel == 0))
		return false;
	else{
		if(m_buffer_vector.size() > 0)
			m_buffer_vector.clear();
		for(int i = 0;i < m_count;i++){
			tmp = (BYTE*)malloc(m_buffer_width * m_buffer_height * m_byteperpixel);
			memset(tmp,0,m_buffer_width * m_buffer_height * m_byteperpixel);
			m_buffer_vector.push_back(tmp);
		}
		return true;
	}
}

BOOL PickPkgContent::CreateBuffer(int width,int height,int count,int byteperpixel){
	BOOL status;
	SetBufferSize(width,height);
	SetBufferCount(count);
	SetBitperpixel(byteperpixel);
	status = CreateBuffer();
	return status;
}

BOOL PickPkgContent::DestroyBuffer(){
	
	BYTE* tmp;
	for( int i = 0;i < m_buffer_vector.size();i++){
		tmp = m_buffer_vector.at(i);
		free(tmp);
	}
	m_buffer_vector.clear();
	if(m_pkg_vector != NULL)
		free(m_pkg_vector);
	m_pkg_vector = NULL;
	return true;
}

BOOL PickPkgContent::ReadBuffer(BYTE* pdst){
	return ReadBuffer(pdst,m_currentIndex);
}

BOOL PickPkgContent::ReadBuffer(BYTE* pdst,int index){
	BYTE* psrc = m_buffer_vector.at(index);
	memcpy(pdst,psrc,m_buffer_width * m_buffer_height * m_byteperpixel);
	return true;
}

void PickPkgContent::ShowData()
{

	int pkgcount = (m_buffer_height * m_buffer_width * m_byteperpixel * (m_skip_pkg + 1)) / m_DataSize;
	//	qDebug() << "1111111111111111111111111111111111111111111111111111111111111111111" << endl;
	if (m_currentPkgIndex > m_PickpkgIndex) {
		if ((m_currentPkgIndex - m_PickpkgIndex) > pkgcount)
		{
			//			qDebug() << "222222222222222222222222222222222222222222222222222222222222222222211" << endl;
			PickData();
			//			qDebug() << "333333333333333333333333333333333333333333333333333333333333333333333" << endl;
		}
	}
	else if (m_currentPkgIndex < m_PickpkgIndex) {
		if ((PKG_COUNT - m_PickpkgIndex + m_currentPkgIndex) > pkgcount) {
			//			qDebug() << "4444444444444444444444444444444444444444444444444444444444444444444" << endl;
			PickData();
			//			qDebug() << "55555555555555555555555555555555555555555555555555555555555555555555" << endl;
		}

	}
}

void PickPkgContent::SetPkgSize(int size){
	m_pkg_size = size;
//	for(int i = 0;i< PKG_COUNT;i++){
	if(m_pkg_vector != NULL)
		free(m_pkg_vector);
	m_pkg_vector= (BYTE*)malloc(m_pkg_size * PKG_COUNT);
	memset(m_pkg_vector, 0, m_pkg_size * PKG_COUNT);
//	}
}

void PickPkgContent::SetRowID(BYTE* id,int len,int position){
	m_RowID.id = (BYTE*)malloc(len);
	memcpy(m_RowID.id,id,len);
	m_RowID.len = len;
	m_RowID.skip = 0;
	m_RowID.position = position;
}

void PickPkgContent::SetPkgID(BYTE* id,int len,int position){
	m_PkgID.id = (BYTE*)malloc(len);
	memcpy(m_PkgID.id,id,len);
	m_PkgID.len = len;
	m_PkgID.skip = 0;
	m_PkgID.position = position;
}

void PickPkgContent::SetPkgFrameID(BYTE* id,int len,int position){
	m_FrameID.id = (BYTE*)malloc(len);
	memcpy(m_FrameID.id,id,len);
	m_FrameID.len = len;
	m_FrameID.position = position;
}
void PickPkgContent::SetDataInfo(int size,int start){
	m_DataSize = size;
	m_DataStartPosition = start;
}

void PickPkgContent::SetBitperpixel(int byte){
	m_byteperpixel = byte;
}

void PickPkgContent::PushPkg(BYTE* src,int count){
//	for(int i = 0; i < count ;i++){
	
	if(m_currentPkgIndex + count >= PKG_COUNT){
		memcpy(&m_pkg_vector[m_currentPkgIndex * m_pkg_size],src,m_pkg_size * (PKG_COUNT - m_currentPkgIndex));
		memcpy(m_pkg_vector,&src[m_pkg_size * (PKG_COUNT - m_currentPkgIndex)],m_pkg_size * (m_currentPkgIndex + count - PKG_COUNT));

//		memcpy(&m_pkg_vector[m_currentPkgIndex * m_pkg_size], src, m_pkg_size * count);
		m_currentPkgIndex = m_currentPkgIndex + count  - PKG_COUNT;
	}
	else{
		memcpy(&m_pkg_vector[m_currentPkgIndex * m_pkg_size],src,m_pkg_size * count);
		
   	    m_currentPkgIndex += count;
	}
}

void PickPkgContent::SetSkipPkg(int count){
	m_skip_pkg = count;
}


void PickPkgContent::PickData(void)
{
	BYTE* psrc;
	BYTE* pdst = m_buffer_vector.at(m_currentIndex);
	int count = 0;
	int height = 0;
	int copy_length = 0;
	//int pixel = (m_bitperpixel / 8);
	LARGE_INTEGER freq_;
	QueryPerformanceFrequency(&freq_);

	LARGE_INTEGER begin_time;
	LARGE_INTEGER end_time;
	QueryPerformanceCounter(&begin_time);
	//TRACE("m_currentIndex %d\n",++m_debug_test);
	if (m_debug_test == 85)
		m_debug_test = 85;
	int pkgcount = m_buffer_height * m_buffer_width * m_byteperpixel * m_skip_pkg / m_DataSize;
	m_PickpkgIndex += pkgcount;
	m_searchpoint_InData = (m_searchpoint_InData + pkgcount * m_DataSize) % m_DataSize;
	int result = FindDataID();
	if (result < 0) {
		m_lasterror = NO_ROWID;
		return;
	}
	m_searchpoint_InData = result;
	psrc = m_pkg_vector + m_PickpkgIndex * m_pkg_size + m_searchpoint_InData + m_RowID.skip + m_DataStartPosition;

	memcpy(&pdst[count], psrc, (m_DataSize - m_searchpoint_InData));
	count += (m_DataSize - m_searchpoint_InData - m_RowID.skip);
	copy_length += (m_DataSize - m_searchpoint_InData - m_RowID.skip);
	//m_searchpoint_InData = 0;
	if (m_PickpkgIndex++ >= PKG_COUNT)
		m_PickpkgIndex = 0;
	psrc = m_pkg_vector + m_PickpkgIndex * m_pkg_size + m_DataStartPosition;


	while (count < m_buffer_height * m_buffer_width * m_byteperpixel) {
	//	if (test == 88384)
	//		test = 88384;
	/*	if (pdst[height*m_buffer_width*m_byteperpixel] != 0xfa) {
			if (pdst[height*m_buffer_width*m_byteperpixel] != 0) {
			
			FILE *file;
			file = fopen("1.raw", "w+b");
			fwrite(pdst, 1, m_buffer_width*m_byteperpixel*m_buffer_height, file);
			fclose(file);
		}
*/
			//pdst[height*m_buffer_width*m_byteperpixel] = 0xfa;
		
		if ((copy_length + m_DataSize) >= m_buffer_width * m_byteperpixel) {
			memcpy(&pdst[count], psrc, (m_buffer_width * m_byteperpixel - copy_length));
			height++;
			test++;
			count += (m_buffer_width * m_byteperpixel - copy_length);
			if (count == m_buffer_height * m_buffer_width * m_byteperpixel)
				break;
			if ((copy_length + m_DataSize) - m_buffer_width * m_byteperpixel >= m_RowID.skip) {
				psrc = m_pkg_vector + m_PickpkgIndex * m_pkg_size + m_DataStartPosition + (m_buffer_width * m_byteperpixel - copy_length) + m_RowID.skip;
				copy_length = (copy_length + m_DataSize) - m_buffer_width * m_byteperpixel - m_RowID.skip;
				memcpy(&pdst[count], psrc, copy_length);
			}
			else {

				if (++m_PickpkgIndex >= PKG_COUNT)
					m_PickpkgIndex = 0;
				copy_length = m_RowID.skip - ((copy_length + m_DataSize) - m_buffer_width * m_byteperpixel);
				psrc = m_pkg_vector + m_PickpkgIndex * m_pkg_size + m_DataStartPosition + abs(copy_length);
				copy_length = m_DataSize - copy_length;
				memcpy(&pdst[count], psrc, copy_length);
				//	m_PickpkgIndex++;
			}

			if (++m_PickpkgIndex >= PKG_COUNT)
				m_PickpkgIndex = 0;
			
			//copy_length = copy_length);
			count += copy_length;
			psrc = m_pkg_vector + m_PickpkgIndex * m_pkg_size + m_DataStartPosition;
		}
		else {
			memcpy(&pdst[count], psrc, m_DataSize);
			count += m_DataSize;
			copy_length += m_DataSize;
			if ((m_PickpkgIndex++) >= PKG_COUNT)
				m_PickpkgIndex = 0;

			psrc = m_pkg_vector + m_PickpkgIndex * m_pkg_size + m_DataStartPosition;
		}
		//if(m_debug_test == 85)
		//	TRACE("m_PickpkgIndex %d,copy_length %d\n",m_PickpkgIndex,copy_length);
	}

	m_searchpoint_InData = count - (m_buffer_height * m_buffer_width * m_byteperpixel);
// 	if (m_searchpoint_InData > 0) {
// 		count = 0;
// 		memcpy(&pdst[count], psrc, m_searchpoint_InData);
// 		count += m_searchpoint_InData;
// 	}
	
	QueryPerformanceCounter(&end_time);

	double ns_time = (end_time.QuadPart - begin_time.QuadPart) * 1000000.0 / freq_.QuadPart;
/*	pdst = m_buffer_vector.at(m_currentIndex);
	for(int i = 0;i<m_buffer_height;i++){
		if (pdst[i*m_buffer_width*m_byteperpixel] != 0xfa) {
			FILE *file;
			file = fopen("1.raw", "w");
			fwrite(pdst, 1,m_buffer_width*m_byteperpixel*m_buffer_height,file);
			fclose(file);
		}
	}
*/
	m_pFunc(m_param,m_spectrl);
	BufferNext();
//	qDebug() << "6666666666666666666666666666666666666666666666666666666666666666666666666666" << endl;
//	ResetPkg();
}
void PickPkgContent::ResetPkg() {
	m_currentPkgIndex = 0;
	m_PickpkgIndex = 0;
	return;
	if (abs(m_PickpkgIndex - m_currentPkgIndex) * m_DataSize < m_buffer_height * m_buffer_width * m_byteperpixel) {
		m_currentPkgIndex = m_PickpkgIndex;
	}
}
void PickPkgContent::BufferNext(){
	m_currentIndex ++;
	if(m_currentIndex >= m_count)
		m_currentIndex = 0;
}

void PickPkgContent::GeneraterBuffer(){

}

int PickPkgContent::FindDataID(){
	int start = 0;
	BYTE* tmp = m_pkg_vector + m_PickpkgIndex * m_pkg_size + m_DataStartPosition;

	while (1)
	{
		if (memcmp(tmp, m_RowID.id, m_RowID.len) != 0)
		{
			tmp++;
			start++; 
			if (start >= (m_DataSize - m_RowID.len)) {
				start = 0;
				m_PickpkgIndex++;
				if (m_PickpkgIndex >= m_currentPkgIndex)
				{
					return -1;
				}
			
				tmp = m_pkg_vector + m_PickpkgIndex * m_pkg_size + m_DataStartPosition;

			}	
			if (m_PickpkgIndex >= PKG_COUNT)
				m_PickpkgIndex = 0;
		}
		else
		{
			break;
			if ((tmp[26] + tmp[27] + tmp[28] + tmp[29]) == 0)
			{
				break;
			}
			else
			{
				start = 0;
				m_PickpkgIndex++;
				if (m_PickpkgIndex >= m_currentPkgIndex)
				{
					return -1;
				}
				if (m_PickpkgIndex >= PKG_COUNT)
					m_PickpkgIndex = 0;
				tmp = m_pkg_vector + m_PickpkgIndex * m_pkg_size + m_DataStartPosition;
			}
		}
	}
// 	while(memcmp(tmp,m_RowID.id,m_RowID.len) != 0 && ((tmp[26] + tmp[27] + tmp[28] + tmp[29]) == 0)){
// 		tmp ++;
// 		start++;
// 		if(start >= m_DataSize){
// 			start = 0;
// 			m_PickpkgIndex++;
// 			if (m_PickpkgIndex == m_currentPkgIndex)
// 			{
// 				return -1;
// 			}
// 			if(m_PickpkgIndex >= PKG_COUNT)
// 				m_PickpkgIndex = 0;
// 			tmp = m_pkg_vector + m_PickpkgIndex * m_pkg_size + m_DataStartPosition;
// 			
// 		}
// 	}
	return start;
}

void PickPkgContent::RegisterCallBack(PickPkgCallbackFunc func,void* param){
	m_param = param;
	m_pFunc = func;
}
UINT PickPkgContent::GetLastError(){
	return m_lasterror;
}