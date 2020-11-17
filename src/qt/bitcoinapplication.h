#ifndef BITCOIN_BITCOINAPPLICATION_H
#define BITCOIN_BITCOINAPPLICATION_H

#include <QApplication>
#include <QString>
#include <QThread>
#include <QTranslator>

#include "clientmodel.h"
#include "net.h"
#include "networkstyle.h"
#include "optionsmodel.h"

#ifdef ENABLE_WALLET
#include "paymentserver.h"
#include "walletmodel.h"
#endif


/** Main PIVX application object */
class BitcoinApplication : public QApplication
{
    Q_OBJECT
public:
    explicit BitcoinApplication(int& argc, char** argv);
    ~BitcoinApplication();

#ifdef ENABLE_WALLET
    /// Create payment server
    void createPaymentServer();
#endif
    /// parameter interaction/setup based on rules
    void parameterSetup();
    /// Create options model
    void createOptionsModel();
    /// Create main window
    void createWindow(const NetworkStyle* networkStyle);
    /// Create splash screen
    void createSplashScreen(const NetworkStyle* networkStyle);

    /// Create tutorial screen
    bool createTutorialScreen();

    /// Request core initialization
    void requestInitialize();
    /// Request core shutdown
    void requestShutdown();

    /// Get process return value
    int getReturnValue() { return returnValue; }

    /// Get window identifier of QMainWindow (PIVXGUI)
    WId getMainWinId() const;

public Q_SLOTS:
    void initializeResult(int retval);
    void shutdownResult(int retval);
    /// Handle runaway exceptions. Shows a message box with the problem and quits the program.
    void handleRunawayException(const QString& message);
    void updateTranslation(bool forceLangFromSettings = false);

Q_SIGNALS:
    void requestedInitialize();
    void requestedRestart(QStringList args);
    void requestedShutdown();
    void stopThread();
    void splashFinished(QWidget* window);

private:
    QThread* coreThread;
    OptionsModel* optionsModel;
    ClientModel* clientModel;
    PIVXGUI* window;
    QTimer* pollShutdownTimer;
#ifdef ENABLE_WALLET
    PaymentServer* paymentServer;
    WalletModel* walletModel;
#endif
    int returnValue;
    QTranslator qtTranslatorBase, qtTranslator, translatorBase, translator;

    void startThread();
};

#endif // BITCOIN_BITCOINAPPLICATION_H
