#include "extendedoptionsmenu.h"
#include "ui_extendedoptionsmenu.h"

ExtendedOptionsMenu::ExtendedOptionsMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExtendedOptionsMenu)
{
    ui->setupUi(this);
}

ExtendedOptionsMenu::~ExtendedOptionsMenu()
{
    delete ui;
}
