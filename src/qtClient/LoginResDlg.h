#ifndef LOGINRESDLG_H_
#define LOGINRESDLG_H_

#include <QWidget>
#include <QDialog>
#include <QLinearGradient>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QStackedWidget;

class LoginResDlg : public QDialog  
{
public: 
    LoginResDlg(int cfd, QWidget* parent = nullptr);
private:  
    void initUi();

    QLinearGradient m_lineGrad;     // 渐近背景颜色
    // 定义矩形
    QRect m_rightRect;
    QRect m_leftRect;
    QRect m_midRect;

    // 两个页面相互切换, 堆布局
    QStackedWidget* m_stk{};

    // 登录页面
    QWidget* createLoginPage();   // 创建登录页面
    QLineEdit* m_userid{};          // 用户id
    QLineEdit* m_usernameEdit{};  // 用户名
    QLineEdit* m_passwordEdit{};  // 密码
    QCheckBox* m_remPwdChx{};     // 勾选：是否记住密码
    QPushButton* m_goRegBtn{};    // 跳转到注册页面
    QPushButton* m_loginBtn{};    // 登陆按钮

    // 注册页面
    QWidget* createRegPage();       // 创建注册页面
    QLineEdit* m_reg_userid{};          // 用户id
    QLineEdit* m_reg_username{};    // 用户名
    QLineEdit* m_reg_pwd{};         // 密码
    QLineEdit* m_reg_rePwd{};       // 确认密码 
    QPushButton* m_reg_goLoginBtn{}; // 转到登录页面
    QPushButton* m_reg_reBtn{};     // 注册

    // 登录，注册
    void Login();
    void Register();

    // --------------------- 客户端 fd
    int m_cfd;

protected:  
    // 从写绘图事件
    void paintEvent(QPaintEvent* ev)override;
};

#endif // !LOGINRESDLG_H_