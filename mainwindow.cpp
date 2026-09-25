bool MainWindow::connectToDatabase() {
    // 加载SQL Server驱动
    db = QSqlDatabase::addDatabase("QODBC");

    // 设置连接参数（根据实际情况修改）
    QString dsn = "DRIVER={SQL Server};SERVER=你的服务器地址;DATABASE=SubwaySystem;UID=你的用户名;PWD=你的密码;";
    db.setDatabaseName(dsn);

    // 尝试连接
    if (db.open()) {
        qDebug() << "数据库连接成功";
        return true;
    } else {
        QMessageBox::critical(this, "连接错误", "无法连接到数据库:\n" + db.lastError().text());
        return false;
    }
}
