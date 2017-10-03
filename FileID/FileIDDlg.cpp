
// FileIDDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FileID.h"
#include "FileIDDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileIDDlg 对话框



CFileIDDlg::CFileIDDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FILEID_DIALOG, pParent)
	, m_filePath(_T(""))
	, m_fileID(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_filePath);
	DDX_Text(pDX, IDC_EDIT2, m_fileID);
}

BEGIN_MESSAGE_MAP(CFileIDDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CFileIDDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFileIDDlg 消息处理程序

BOOL CFileIDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFileIDDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFileIDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFileIDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 为了适应将字符串转化为唯一的数字ID，调整字符串
// 如果字符大写，就转化为小写，将'\\'转化为'/'
static char* StringAdjust(char* p)
{
	for (int i = 0; p[i]; i++)
	{
		if (p[i] >= 'A' && p[i] <= 'Z')
			p[i] += 'a' - 'A';
		else if (p[i] == '/')
			p[i] = '\\';
	}
	return p;
}

typedef size_t uint32;
typedef size_t uint;
typedef unsigned char uint8;
typedef int int32;

#define mix(a,b,c) \
			{ \
				a -= b; a -= c; a ^= (c>>13); \
				b -= c; b -= a; b ^= (a<<8); \
				c -= a; c -= b; c ^= (b>>13); \
				a -= b; a -= c; a ^= (c>>12);  \
				b -= c; b -= a; b ^= (a<<16); \
				c -= a; c -= b; c ^= (b>>5); \
				a -= b; a -= c; a ^= (c>>3);  \
				b -= c; b -= a; b ^= (a<<10); \
				c -= a; c -= b; c ^= (b>>15); \
			}

inline uint32 hash(
	register uint8 *k,        /* the key */
	register uint32  length,   /* the length of the key */
	register uint32  initval    /* the previous hash, or an arbitrary value */
)
{
	register uint32 a, b, c, len;

	/* Set up the internal state */
	len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = initval;           /* the previous hash value */

						   /*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a += (k[0] + ((uint32)k[1] << 8) + ((uint32)k[2] << 16) + ((uint32)k[3] << 24));
		b += (k[4] + ((uint32)k[5] << 8) + ((uint32)k[6] << 16) + ((uint32)k[7] << 24));
		c += (k[8] + ((uint32)k[9] << 8) + ((uint32)k[10] << 16) + ((uint32)k[11] << 24));
		mix(a, b, c);
		k += 12;
		len -= 12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c += length;
	switch (len)             /* all the case statements fall through */
	{
	case 11:
		c += ((uint32)k[10] << 24);
	case 10:
		c += ((uint32)k[9] << 16);
	case 9:
		c += ((uint32)k[8] << 8);
		/* the first byte of c is reserved for the length */
	case 8:
		b += ((uint32)k[7] << 24);
	case 7:
		b += ((uint32)k[6] << 16);
	case 6:
		b += ((uint32)k[5] << 8);
	case 5:
		b += k[4];
	case 4:
		a += ((uint32)k[3] << 24);
	case 3:
		a += ((uint32)k[2] << 16);
	case 2:
		a += ((uint32)k[1] << 8);
	case 1:
		a += k[0];
		/* case 0: nothing left to add */
	}
	mix(a, b, c);
	/*-------------------------------------------- report the result */
	return c;
}

uint32 String2ID(const char* str)
{
	if (str == NULL)
		return 0;
	return hash((uint8*)str, (uint32)strlen(str), 0);
}

unsigned int FileName2Id(const char *filename)
{
	char buffer[256];
	buffer[255] = 0;
	strncpy_s(buffer, filename, 255);
	return (unsigned int)String2ID(StringAdjust(buffer));
}

void CFileIDDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	UpdateData();
	m_fileID.Format("%d",(FileName2Id(m_filePath.GetString())));
	UpdateData(false);
}
