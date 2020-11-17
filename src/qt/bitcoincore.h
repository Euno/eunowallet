#ifndef BITCOIN_BITCOINCORE_H
#define BITCOIN_BITCOINCORE_H

#include <exception>

#include <QObject>
#include <QString>


/** Class encapsulating PIVX Core startup and shutdown.
 * Allows running startup and shutdown in a different thread from the UI thread.
 */
class BitcoinCore : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinCore();

public Q_SLOTS:
    void initialize();
    void shutdown();
    void restart(QStringList args);

Q_SIGNALS:
    void initializeResult(int retval);
    void shutdownResult(int retval);
    void runawayException(const QString& message);

private:
    /// Flag indicating a restart
    bool execute_restart;

    /// Pass fatal exception message to UI thread
    void handleRunawayException(const std::exception* e);
};

#endif // BITCOIN_BITCOINCORE_H
