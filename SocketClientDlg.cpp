// SocketClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include <comdef.h>
#include <atlbase.h>
#include "SocketServer.h"
#include "AboutBox.h"
#include "SocketClientDlg.h"

const int AF_IPV4   = 0;
const int AF_IPV6   = 1;
const int SOCK_TCP  = SOCK_STREAM-1;
const int SOCK_UDP  = SOCK_DGRAM-1;

// CSocketClientDlg dialog
CSocketClientDlg::CSocketClientDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSocketClientDlg::IDD, pParent)
    , m_nMode(AF_IPV4)
    , m_nSockType(SOCK_TCP)
    , m_strPort(_T("2000"))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_SocketClient.SetInterface(this);
}

CSocketClientDlg::~CSocketClientDlg()
{
}

void CSocketClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_SVR_PORT, m_strPort);
    DDX_Radio(pDX, IDC_IPV4, m_nMode);
    DDX_Radio(pDX, IDC_TCP, m_nSockType);
    DDX_Control(pDX, IDC_TXT_MESSAGE, m_ctlMessage);
    DDX_Control(pDX, IDC_MESSAGE_LIST, m_ctlMsgList);
}

BOOL CSocketClientDlg::PreTranslateMessage(MSG* pMsg)
{
    if ( pMsg->message == WM_KEYDOWN )
    {
        if ( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
        {
            if ( pMsg->wParam == VK_RETURN )
            {
                TCHAR szClassName[40] = { 0 };
                CWnd* pWnd = GetFocus();
                if ( pWnd )
                {
                    GetClassName(pWnd->GetSafeHwnd(), szClassName, 40);
                    if ( lstrcmpi(_T("EDIT"), szClassName) == 0 )
                    {
                        return FALSE;
                    }
                }
            }
            return TRUE;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CSocketClientDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_COMMAND_RANGE(IDC_IPV4, IDC_IPV6, OnModeSelected)
    ON_BN_CLICKED(IDC_BTN_START, &CSocketClientDlg::OnBnClickedBtnStart)
    ON_BN_CLICKED(IDC_BTN_STOP, &CSocketClientDlg::OnBnClickedBtnStop)
    ON_BN_CLICKED(IDC_BTN_SEND, &CSocketClientDlg::OnBnClickedBtnSend)
END_MESSAGE_MAP()


// CSocketClientDlg message handlers

BOOL CSocketClientDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here
    TCHAR szLocalAddr[MAX_PATH] = { 0 };
    int nFamily = (m_nMode == AF_IPV4) ? AF_INET : AF_INET6;
    CSocketHandle::GetLocalAddress(szLocalAddr, MAX_PATH, nFamily);
    SetDlgItemText(IDC_IPADDRESS, szLocalAddr);
    SendDlgItemMessage(IDC_SVR_PORTINC, UDM_SETRANGE32, 2000, 64000);
    m_ctlMsgList.SetLimitText( 1*1024*1024 );
    m_ctlMessage.SetFocus();
    SyncControls();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSocketClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSocketClientDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSocketClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CSocketClientDlg::OnDestroy()
{
    m_SocketClient.Terminate();
    CDialog::OnDestroy();
}

void CSocketClientDlg::EnableControl(UINT nCtrlID, BOOL bEnable)
{
    GetDlgItem(nCtrlID)->EnableWindow(bEnable);
}

void CSocketClientDlg::SyncControls()
{
    bool bOpen = m_SocketClient.IsOpen();
    EnableControl(IDC_BTN_START, !bOpen);
    EnableControl(IDC_BTN_STOP, bOpen);
    EnableControl(IDC_BTN_SEND, bOpen);
    EnableControl(IDC_IPV4, !bOpen);
    EnableControl(IDC_IPV6, !bOpen);
    EnableControl(IDC_TCP, !bOpen);
    EnableControl(IDC_UDP, !bOpen);
}

void CSocketClientDlg::GetAddress(const SockAddrIn& addrIn, CString& rString) const
{
    TCHAR szIPAddr[MAX_PATH] = { 0 };
    CSocketHandle::FormatIP(szIPAddr, MAX_PATH, addrIn);
    rString.Format(_T("%s : %d"), szIPAddr, static_cast<int>(static_cast<UINT>(ntohs(addrIn.GetPort()))) );
}

void CSocketClientDlg::AppendText(LPCTSTR lpszFormat, ...)
{
    if ( !::IsWindow(m_ctlMsgList.GetSafeHwnd()) ) return;
    TCHAR szBuffer[512];
    HWND hWnd = m_ctlMsgList.GetSafeHwnd();
    DWORD dwResult = 0;
    if (SendMessageTimeout(hWnd, WM_GETTEXTLENGTH, 0, 0, SMTO_NORMAL, 500L, &dwResult) != 0)
    {
        int nLen = (int) dwResult;
        if (SendMessageTimeout(hWnd, EM_SETSEL, nLen, nLen, SMTO_NORMAL, 500L, &dwResult) != 0)
        {
            size_t cb = 0;
            va_list args;
            va_start(args, lpszFormat);
            ::StringCchVPrintfEx(szBuffer, 512, NULL, &cb, 0, lpszFormat, args);
            va_end(args);
            SendMessageTimeout(hWnd, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(szBuffer), SMTO_NORMAL, 500L, &dwResult);
        }
    }
}

bool CSocketClientDlg::GetDestination(SockAddrIn& addrIn) const
{
    CString strIPAddress, strPort;
    GetDlgItemText(IDC_IPADDRESS, strIPAddress);
    GetDlgItemText(IDC_SVR_PORT, strPort);
    int nFamily = (m_nMode == AF_IPV4) ? AF_INET : AF_INET6;
    return addrIn.CreateFrom(strIPAddress, strPort, nFamily);
}

bool CSocketClientDlg::SetupMCAST()
{
    const TCHAR szIPv4MCAST[] = TEXT("239.121.1.2");
    const TCHAR szIPv6MCAST[] = TEXT("FF02:0:0:0:0:0:0:1"); // All Nodes local address
    bool result = false;
    if ( m_nSockType == SOCK_UDP )
    {
        if ( m_nMode == AF_IPV4 ) {
            result = m_SocketClient->AddMembership(szIPv4MCAST, NULL);
        } else {
            result = m_SocketClient->AddMembership(szIPv6MCAST, NULL);
            HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
            hr = hr;
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////

void CSocketClientDlg::OnThreadBegin(CSocketHandle* pSH)
{
    ASSERT( pSH == m_SocketClient );
    (pSH);
    CString strAddr;
    SockAddrIn sockAddr;
    m_SocketClient->GetSockName(sockAddr);
    GetAddress( sockAddr, strAddr );
    AppendText( _T("Client Running on: %s\r\n"), strAddr);
}

void CSocketClientDlg::OnThreadExit(CSocketHandle* pSH)
{
    ASSERT( pSH == m_SocketClient );
    (pSH);
}

void CSocketClientDlg::OnDataReceived(CSocketHandle* pSH, const BYTE* pbData, DWORD dwCount, const SockAddrIn& addr)
{
    ASSERT( pSH == m_SocketClient );
    (pSH);
    CString strAddr, strText;
    USES_CONVERSION;
    LPTSTR pszText = strText.GetBuffer(dwCount+1);
    ::StringCchCopyN(pszText, dwCount+1, A2CT(reinterpret_cast<LPCSTR>(pbData)), dwCount);
    strText.ReleaseBuffer();
    GetAddress( addr, strAddr );
    AppendText( _T("%s>(%s)\r\n"), strAddr, strText);
}

void CSocketClientDlg::OnConnectionDropped(CSocketHandle* pSH)
{
    ASSERT( pSH == m_SocketClient );
    (pSH);
    AppendText( _T("Connection lost with server. Need restart.\r\n") );
}

void CSocketClientDlg::OnConnectionError(CSocketHandle* pSH, DWORD dwError)
{
    ASSERT( pSH == m_SocketClient );
    (pSH);
    _com_error err(dwError);
    AppendText( _T("Communication Error:\r\n%s\r\n"), err.ErrorMessage() );
}

void CSocketClientDlg::OnModeSelected(UINT nId)
{
    m_nMode = nId - IDC_IPV4;
    TCHAR szLocalAddr[MAX_PATH] = { 0 };
    int nFamily = (m_nMode == AF_IPV4) ? AF_INET : AF_INET6;
    CSocketHandle::GetLocalAddress(szLocalAddr, MAX_PATH, nFamily);
    SetDlgItemText(IDC_IPADDRESS, szLocalAddr);
}

void CSocketClientDlg::OnBnClickedBtnStart()
{
    UpdateData(TRUE);
    CString strAddr, strPort;
    GetDlgItemText(IDC_IPADDRESS, strAddr);
    GetDlgItemText(IDC_SVR_PORT, strPort);
    int nFamily = (m_nMode == AF_IPV4) ? AF_INET : AF_INET6;
    if ( !m_SocketClient.StartClient(NULL, strAddr, strPort, nFamily, (m_nSockType+1) ) )
    {
        MessageBox(_T("Failed to start client connection."), NULL, MB_ICONSTOP);
    }
    else
    {
        SetupMCAST();
    }
    SyncControls();
}

void CSocketClientDlg::OnBnClickedBtnStop()
{
    m_SocketClient.Terminate();
    SyncControls();
}

void CSocketClientDlg::OnBnClickedBtnSend()
{
   if ( m_SocketClient.IsOpen() )
    {
        CString strMsg;
        m_ctlMessage.GetWindowText( strMsg );
        if ( strMsg.IsEmpty() ) {
            AppendText( _T("Please enter the message to send.\r\n") );
            return;
        }
        USES_CONVERSION;
        if (m_nSockType == SOCK_TCP)
        {
            m_SocketClient.Write((const LPBYTE)(T2CA(strMsg)), strMsg.GetLength(), NULL);
        }
        else
        {
            SockAddrIn sockAddr;
            GetDestination(sockAddr);
            m_SocketClient.Write((const LPBYTE)(T2CA(strMsg)), strMsg.GetLength(), sockAddr);
        }
    }
    else
    {
        MessageBox(_T("Socket is not connected"));
    }
}
