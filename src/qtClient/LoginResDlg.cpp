#include "LoginResDlg.h"
#include "EventFilterObject.h"
#include <QPaintEvent>
#include <QPainter>
#include <QGraphicsDropShadowEffect>  // 设置阴影
#include <QStaticText>
#include <QPushButton>
#include <QFile>
#include <QLineEdit>
#include <QCheckBox>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QMessageBox>

#include <string>
#include <sys/types.h>
#include <sys/socket.h>

// json 
#include "json.hpp"
#include "resultCode.h"
#include "storeMess.h"
using json = nlohmann::json;


LoginResDlg::LoginResDlg(int cfd, QWidget* parent)
    : QDialog(parent)
    , m_cfd(cfd)
{
    initUi();
}

void LoginResDlg::initUi()
{
    // 去边框
    setWindowFlag(Qt::FramelessWindowHint);
    // 设置透明背景
    setAttribute(Qt::WA_TranslucentBackground);

    // 安装过滤器
    installEventFilter(new EventFilterObject(this));

    // 调整窗口大小
    setFixedSize(740, 470);

    // 关闭按钮
    auto closeBtn = new QPushButton(this);
    closeBtn->setObjectName("closeBtn");  // 设置对象名, 可以用css修饰
    closeBtn->move(width() - 60, y() + 50);   // 移动
    closeBtn->clearFocus();   // 确保鼠标停在这个按钮上的时候，不影响其他控件输入
    closeBtn->setFocusPolicy(Qt::NoFocus);  
    // 关闭实现
    connect(closeBtn, &QPushButton::clicked, this, &LoginResDlg::close);  // 调用QWidget的方法

    // 初始化矩形页面位置，方便后面绘制
    m_rightRect = {0, 40, width() - 2 * 5, height() - 40 * 2};
    m_leftRect = {0, 0, 185, height() - 2 * 5};
    m_midRect = {0, 0, 370, height() - 2 * 5};  // 画矩形是通过矩形画

    // 初始化线性渐变
    m_lineGrad.setStart(m_midRect.x(), m_midRect.y());  // 弦 设置渐变渐变
    m_lineGrad.setFinalStop(m_midRect.right(), m_midRect.bottom()); 
    m_lineGrad.setColorAt(0, QColor(0xe0c3fc));
    m_lineGrad.setColorAt(1, QColor(0x8ec5fc));

    // 设置边框阴影阴影
    auto effect = new QGraphicsDropShadowEffect(this);
    effect->setOffset(5);
    effect->setBlurRadius(10);
    setGraphicsEffect(effect);


    // ----------------------- 页面 创建
    m_stk = new QStackedWidget(this);
	m_stk->setGeometry(434, 150, 200, 260);
    m_stk->addWidget(createLoginPage());   // 登录
    m_stk->addWidget(createRegPage());     // 注册

    // ---- 设置css样式
    QFile file(":/Resource/style.css");
    if(file.open(QIODevice::ReadOnly)) {
        setStyleSheet(file.readAll());  // 设置样式
    }
}

void LoginResDlg::paintEvent(QPaintEvent* ev)
{
    QPainter painter(this);
    // 画背景矩形, 左右边框
    painter.fillRect(m_rightRect, Qt::white);
    painter.fillRect(m_leftRect, m_lineGrad);  

    // 画弦
    painter.setPen(Qt::NoPen);  // 设置画笔
    painter.setBrush(m_lineGrad); // 画刷
    painter.drawChord(m_midRect, -90 * 16, 180 * 16);

    // 绘制文本
    painter.setPen(QPen(Qt::white,1,Qt::PenStyle::SolidLine));  // 画笔
    painter.setBrush(Qt::white);  // 画刷
    painter.setFont(QFont("微软雅黑", 30));
    painter.drawStaticText(48, 60, QStaticText("Welcome"));

    painter.setFont(QFont("微软雅黑", 15));
    painter.drawStaticText(52, 120, QStaticText("基于集群的即时通讯(@yxz)"));

    painter.setPen(QColor(0xe0, 0xc3, 0xfc));
    painter.setFont(QFont("宋体", 30));
    painter.drawStaticText(423, 75, QStaticText("LUCKYING"));

    // 绘制图片
    painter.drawPixmap(QRect(40, 200, 250, 250), QPixmap(":/Resource/cai.png"));
    painter.drawPixmap(QRect(40, 200, 250, 250), QPixmap(":/Resource/dream.png"));
}

// 创建登录页面
QWidget* LoginResDlg::createLoginPage()
{
    QWidget* w = new QWidget;
    m_usernameEdit = new QLineEdit;
    m_passwordEdit = new QLineEdit;
    m_userid = new QLineEdit;
    m_remPwdChx = new QCheckBox("记住密码");
    m_goRegBtn = new QPushButton("注册");
    m_loginBtn = new QPushButton("登录");

    // 页面布局：记住密码、注册在同一行，故：水平布局
    auto hlayout = new QHBoxLayout;
    hlayout->addWidget(m_remPwdChx);
    hlayout->addWidget(m_goRegBtn);

    // 垂直布局, 所有按键
    auto vlayout = new QVBoxLayout(w);
    vlayout->addWidget(m_userid);           // id
    vlayout->addWidget(m_usernameEdit);     // 用户名
    vlayout->addWidget(m_passwordEdit);     // 密码
    vlayout->addLayout(hlayout);            // 密码、注册
    vlayout->addWidget(m_loginBtn);         // 登录

    // 设置默认字体
    m_userid->setPlaceholderText("请输入id(1开始)");
    m_usernameEdit->setPlaceholderText("用户名");
    m_passwordEdit->setPlaceholderText("密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    // 设置对象名
    m_goRegBtn->setObjectName("goRegBtn");
    m_loginBtn->setObjectName("loginBtn");  

    // 点击注册，切换注册页面
    connect(m_goRegBtn, &QPushButton::clicked, this, [this] {
        m_stk->setCurrentIndex(1);
    });

    // 点击登录按钮
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginResDlg::Login);

    return w;
}

// 创建注册页面
QWidget* LoginResDlg::createRegPage()
{
    QWidget* w = new QWidget;
    m_reg_userid = new QLineEdit;
    m_reg_username = new QLineEdit;
    m_reg_pwd = new QLineEdit;
    m_reg_rePwd = new QLineEdit;    
    m_reg_goLoginBtn = new QPushButton("登录");
    m_reg_reBtn = new QPushButton("注册");

    // 页面布局：注册、登录在同一行，故：水平布局
    auto hlayout = new QHBoxLayout;
    hlayout->setSpacing(0);
    hlayout->addWidget(m_reg_reBtn);
    hlayout->addWidget(m_reg_goLoginBtn);

    // 垂直布局, 所有按键
    auto vlayout = new QVBoxLayout(w);
    vlayout->addWidget(m_reg_userid);       // id
    vlayout->addWidget(m_reg_username);     // 用户名
    vlayout->addWidget(m_reg_pwd);     // 密码
    vlayout->addWidget(m_reg_rePwd);     // 再次输入密码
    vlayout->addLayout(hlayout);            // 密码、注册

    // 设置默认字
    m_reg_userid->setPlaceholderText("请输入用户id(1开始)");
    m_reg_username->setPlaceholderText("请输入用户名");
    m_reg_pwd->setPlaceholderText("请输入密码");
    m_reg_rePwd->setPlaceholderText("再次输入密码");
    // 密码隐藏
    m_reg_pwd->setEchoMode(QLineEdit::Password);
    m_reg_rePwd->setEchoMode(QLineEdit::Password);
    // 设置对象名字
    m_reg_goLoginBtn->setObjectName("goLoginBtn");
    m_reg_reBtn->setObjectName("regBtn");
    

    // 点击注册，切换注册页面
    connect(m_reg_goLoginBtn, &QPushButton::clicked, this, [this] {
        m_stk->setCurrentIndex(0);
    });

    // 点击注册按钮
    connect(m_reg_reBtn, &QPushButton::clicked, this, &LoginResDlg::Register);

    return w;
}

// 登录, 服务器请求json: {"msgId": 1, "userId": 1, "username": "yxz", "password": "123456"}
void LoginResDlg::Login()
{
    // 获取用户名密码
    int id = m_userid->text().toInt();  
    std::string name = m_usernameEdit->text().toStdString();
    std::string password = m_passwordEdit->text().toStdString();

    // 检查
    if(id == 0) {
        QMessageBox::warning(nullptr, "waring", "请输入正确的id");
    }
    if(name.empty()) {
        QMessageBox::warning(nullptr, "waring", "请输入用户用户名");
    }
    if(password.empty()) {
        QMessageBox::warning(nullptr, "waring", "请输入密码");
    }

    // json, 注意: 和服务器保持一致
    json js;
    js["msgId"] = LOGIN_MSG;
    js["userId"] = id;
    js["username"] = name;
    js["password"] = password;

    //std::cout << "login: " << js.dump() << std::endl;

    // 发送
    send(m_cfd, js.dump().c_str(), js.dump().size(), 0);

    // 收到回复, 如果信号量没有>0，即没有收到数据，则等待
    /*
    main msgType: 2 json: {"code":2,"codeStr":"LoginSuccess",
    "data":{"friendList":["{\"id\":2,\"name\":\"yy\",\"state\":\"offline\"}","{\"id\":3,\"name\":\"ww\",\"state\":\"offline\"}"],
        "groups":["{\"groupId\":1,\"groupdesc\":\"wy\",\"groupname\":\"wxg\",\"users\":[\"{\\\"name\\\":\\\"yxz\\\",\\\"role\\\":\\\"normal\\\",\\\"state\\\":\\\"online\\\",\\\"userId\\\":1}\",\"{\\\"name\\\":\\\"yy\\\",\\\"role\\\":\\\"normal\\\",\\\"state\\\":\\\"offline\\\",\\\"userId\\\":2}\"]}"],
        "userId":1,
        "username":"yxz"},
    "message":"the user login success!!","msgId":2}
    */
    if(sem_wait(&StoreMess::getInstall()->m_loginSem) == 0) {  
        if(StoreMess::getInstall()->m_ifLogin) {
            // 收到回复了
            json rjs = json::parse(StoreMess::getInstall()->m_loginMess);
            // 是否是 success 消息, 具体见 resultCode.h 文件中
            if(rjs["code"].get<int>() == 2) {
                //获取用户名、密码、id
                int rid = rjs["data"]["userId"].get<int>();
                std::string rusername = rjs["data"]["username"].get<std::string>();

                // 验证用户名、密码
                if(rid == id && rusername == name) {
                    //QMessageBox::information(nullptr, "login", "登录成功");  // 测试

                    // --------- 存储每一位好友的离线消息
                    if(rjs["data"].contains("OfflineMessage")) {  // 存在
                        // 插入, 数据格式： id:mess
                        StoreMess::getInstall()->m_loginFriendMess = rjs["data"]["OfflineMessage"].get<std::vector<std::string>>();

                        std::cout << rjs["data"]["OfflineMessage"] << std::endl;
                    }

                    //StoreMess::m_addFriendMess

                    // 退出
                    close();
                }
            } else {
                QMessageBox::warning(nullptr, "login", "id/用户名/密码输入有误");
                std::cerr << rjs["message"].get<std::string>() << std::endl;
            }
        }
    }
}
// 注册
void LoginResDlg::Register()
{
     // 获取id、用户名、密码
    int id = m_reg_userid->text().toInt();  
    std::string name = m_reg_username->text().toStdString();
    std::string password = m_reg_pwd->text().toStdString();
    std::string repassword = m_reg_rePwd->text().toStdString();

    // 检查
    if(id == 0) {
        QMessageBox::warning(nullptr, "waring", "请输入正确的id");
    }
    if(name.empty()) {
        QMessageBox::warning(nullptr, "waring", "请输入用户用户名");
    }
    if(password.empty()) {
        QMessageBox::warning(nullptr, "waring", "请输入密码");
    }
    if(repassword.empty()) {
        QMessageBox::warning(nullptr, "waring", "请再次输入密码");
    }
    if(password != repassword) {
        QMessageBox::warning(nullptr, "waring", "两次输入密码不正确");
    }

    // json, 注意: 和服务器保持一致
    json js;
    js["msgId"] = REGISTER_MSG;
    js["id"] = id;
    js["username"] = name;
    js["password"] = password;

    //std::cout << "register: " << js.dump() << std::endl;

    // 发送
    send(m_cfd, js.dump().c_str(), js.dump().size(), 0);

    // 收到回复, 如果信号量没有>0，即没有收到数据，则等待, 和登录原理一样
    if(sem_wait(&StoreMess::getInstall()->m_registerSem) == 0) {  
        if(StoreMess::getInstall()->m_isRegister) {   // 注册
            // 可以下次注册
            StoreMess::getInstall()->m_isRegister = false;

            // 收到回复了
            json rjs = json::parse(StoreMess::getInstall()->m_registerMess);
            // 是否是 success 消息, 具体见 resultCode.h 文件中
            if(rjs["code"].get<int>() == 3) {
               // 注册消息成功
               QMessageBox::information(nullptr, "success", rjs["message"].get<std::string>().c_str());
            } else {
                QMessageBox::warning(nullptr, "failed", rjs["message"].get<std::string>().c_str());
            }
        }
    }
}

// 