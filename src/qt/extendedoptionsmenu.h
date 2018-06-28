#ifndef EXTENDEDOPTIONSMENU_H
#define EXTENDEDOPTIONSMENU_H

#include <QWidget>

namespace Ui {
class ExtendedOptionsMenu;
}

class ExtendedOptionsMenu : public QWidget
{
    Q_OBJECT

public:
    explicit ExtendedOptionsMenu(QWidget *parent = 0);
    ~ExtendedOptionsMenu();

private:
    Ui::ExtendedOptionsMenu *ui;
};

#endif // EXTENDEDOPTIONSMENU_H
