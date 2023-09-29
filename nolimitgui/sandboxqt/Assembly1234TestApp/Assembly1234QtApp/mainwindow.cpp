#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <AssemblyLib.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->m_CButton, SIGNAL(clicked()), this, SLOT(slotCButtonClicked()));
    connect(ui->m_CppButton, SIGNAL(clicked()), this, SLOT(slotCppButtonClicked()));
    connect(ui->m_AssemblyButton, SIGNAL(clicked()), this, SLOT(slotAssemblyButtonClicked()));
    connect(ui->m_AssemblyReverseButton, SIGNAL(clicked()), this, SLOT(slotAssemblyReverseButtonClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotCButtonClicked()
{
    ui->m_OutputTextLabel->setText(getCText());
}

void MainWindow::slotCppButtonClicked()
{
    ui->m_OutputTextLabel->setText(getCppText());
}

void MainWindow::slotAssemblyButtonClicked()
{
    ui->m_OutputTextLabel->setText(QString::number(get_assembly_test_num()));
}

void MainWindow::slotAssemblyReverseButtonClicked()
{
#if defined(TARGET_OS_ANDROID)
    // android has 2 assembly files (.s and .S) and one of them contains get_assembly_reverse_num
    ui->m_OutputTextLabel->setText(QString::number(get_assembly_reverse_num()));
#else
    // linux/windows has 1 .asm assembly file so fake the missing get_assembly_reverse_num
    ui->m_OutputTextLabel->setText("4321");
#endif // defined(TARGET_OS_ANDROID)
}
