// LoginWindow.cpp
#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include <QMessageBox>
#include <QDebug>
LoginWindow::LoginWindow(QWidget *parent) : QDialog(parent), ui(new Ui::LoginWindow) {
    ui->setupUi(this);
    qDebug() << "数据库连接状态:" << db.open();  // 新增此行
    if (!db.open()) {
        qDebug() << "数据库错误:" << db.lastError().text();
    }
}
LoginWindow::LoginWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow),
    currentUserId(-1)
{
    ui->setupUi(this);
    setWindowTitle("地铁查询售票系统 - 登录");

    // 配置数据库连接（需根据实际情况修改）
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("DRIVER={SQL Server};SERVER=localhost;DATABASE=SubwaySystem;Trusted_Connection=Yes;");

    if (!db.open()) {
        QMessageBox::critical(this, "数据库错误", "连接数据库失败:\n" + db.lastError().text());
    }
}

LoginWindow::~LoginWindow() {
    delete ui;
}

int LoginWindow::getCurrentUserId() const {
    return currentUserId;
}

void LoginWindow::on_loginButton_clicked() {
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        ui->errorLabel->setText("用户名和密码不能为空");
        return;
    }

    // 简单数据库查询验证用户
    QSqlQuery query;
    query.prepare("SELECT userID FROM Users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (query.exec() && query.next()) {
        currentUserId = query.value(0).toInt();
        accept(); // 登录成功，关闭窗口
    } else {
        ui->errorLabel->setText("用户名或密码错误");
        qDebug() << "登录失败: " << query.lastError().text();
    }
}
