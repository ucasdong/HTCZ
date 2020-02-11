#include "PickPkg.h"

PickPkg::PickPkg()
{
	m_bFinishOnePkg = false;
	m_PickCopyCount = 0;
	m_iCurrentModel = 0;
}


PickPkg::~PickPkg()
{
}

void PickPkg::SetRowID(BYTE* id, int len, int position)
{
	m_RowID.id = (BYTE*)malloc(len);
	memcpy(m_RowID.id, id, len);
	m_RowID.len = len;
	m_RowID.skip = 0;
	m_RowID.position = position;
}

void PickPkg::SetPkgID(BYTE* id, int len, int position)
{
	m_PkgID.id = (BYTE*)malloc(len);
	memcpy(m_PkgID.id, id, len);
	m_PkgID.len = len;
	m_PkgID.skip = 0;
	m_PkgID.position = position;
}

void PickPkg::SetPkgFrameID(BYTE* id, int len, int position)
{
	m_FrameID.id = (BYTE*)malloc(len);
	memcpy(m_FrameID.id, id, len);
	m_FrameID.len = len;
	m_FrameID.position = position;
}

void PickPkg::SetBufferSize(int x, int y)
{
	m_buffer_width = x;
	m_buffer_height = y;
}

void PickPkg::SetBufferCount(int count)
{
	m_count = count;
}

void PickPkg::SetBitperpixel(int byte)
{
	m_byteperpixel = byte;
}

bool PickPkg::CreateBuffer(int width, int height, int count, int byteperpixel)
{
	bool status;
	SetBufferSize(width, height);
	SetBufferCount(count);
	SetBitperpixel(byteperpixel);
	status = CreateBuffer();
	return status;
}

bool PickPkg::CreateBuffer()
{
	BYTE* tmp = NULL;
	if ((m_count == 0) || (m_buffer_width == 0) || (m_buffer_height == 0) || (m_byteperpixel == 0))
	{
		return false;
	}		
	else 
	{
		if (m_buffer_vector.size() > 0)
		{
			m_buffer_vector.clear();
		}
		for (int i = 0; i < m_count; i++)
		{
			tmp = (BYTE*)malloc(m_buffer_width * m_buffer_height * m_byteperpixel);
			memset(tmp, 0, m_buffer_width * m_buffer_height * m_byteperpixel);
			m_buffer_vector.push_back(tmp);
		}
		return true;
	}
}

void PickPkg::SetDataInfo(int iAllLen, int start, int iValidSize)
{
	m_pkg_size = iAllLen;       //1024
	m_DataStartPosition = start;//10
	m_DataSize = iValidSize;    //1012
	
// 	if (m_pkg_vector != NULL)
// 	{
// 		free(m_pkg_vector);
// 		m_pkg_vector = NULL;
// 	}
// 
// 	m_pkg_vector = (BYTE*)malloc(m_DataSize * PKG_COUNT);
// 	memset(m_pkg_vector, 0, m_DataSize * PKG_COUNT);
}

void PickPkg::RegisterCallBack(PickPkgCallbackFunc func, void* param)
{
	m_param = param;
	m_pFunc = func;
}

void PickPkg::PushPkg(BYTE* src, int count)
{
	if (memcmp(src, m_PkgID.id, m_PkgID.len) != 0)//数据包头不对
	{
		qDebug() << "ccsds header error" << endl;
		return;
	}
	BYTE* tmp = src + m_DataStartPosition;
	if (m_PickCopyCount == 0)//找头
	{
		bool m_bFindHeader = false;
		int start = 0;
		while (1)
		{
			if (memcmp(tmp, m_RowID.id, m_RowID.len) != 0)
			{
				tmp++;
				start++;
				if (start > m_DataSize - m_RowID.len)
				{
					m_bFindHeader = false;
					break;
				}
			}
			else
			{
				if (m_iCurrentModel == 0)
				{
					if ((tmp[26] + tmp[27] + tmp[28] + tmp[29]) == 0)
					{
						m_bFindHeader = true;
						break;
					}
					else
					{
						m_bFindHeader = false;
						break;
					}
				}
				else if (m_iCurrentModel == 1)
				{
					m_bFindHeader = true;
					break;
				}
			}
		}
		if (m_bFindHeader)
		{
			memcpy(m_buffer_vector[0] + m_PickCopyCount, tmp, m_DataSize - start);
			m_PickCopyCount += m_DataSize - start;
		}
	}
	else 
	{
		int needCopySize = m_buffer_width * m_buffer_height * m_byteperpixel - m_PickCopyCount;
		if (needCopySize > m_DataSize)
		{
			memcpy(m_buffer_vector[0] + m_PickCopyCount, tmp, m_DataSize);
			m_PickCopyCount += m_DataSize;
		}
		else//完成一幅图片
		{
			memcpy(m_buffer_vector[0] + m_PickCopyCount, tmp, needCopySize);
			//m_PickCopyCount += needCopySize;
			m_pFunc(m_param, m_spectrl);

			m_PickCopyCount = 0;		
			memset(m_buffer_vector[0], 0x00, m_buffer_width * m_buffer_height * m_byteperpixel);
			if (m_iCurrentModel == 0)
			{
				tmp += needCopySize;
				if (memcmp(tmp, m_RowID.id, m_RowID.len) == 0)//新一行的开始
				{				
					if ((tmp[26] + tmp[27] + tmp[28] + tmp[29]) == 0)
					{
						memcpy(m_buffer_vector[0] + m_PickCopyCount, tmp, m_DataSize - needCopySize);
						m_PickCopyCount += m_DataSize - needCopySize;
					}
				}
			}
		}
	}
}

bool PickPkg::ReadBuffer(BYTE* pdst)
{
	memcpy(pdst, m_buffer_vector[0], m_buffer_width * m_buffer_height * m_byteperpixel);
	return true;
}

void PickPkg::changeModel(int iModel)
{
	m_iCurrentModel = iModel;
}
