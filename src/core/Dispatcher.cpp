#include <QEvent>
#include <QObject>
#include <QCoreApplication>
#include "Dispatcher.h"

using namespace Robomongo;

Dispatcher::Dispatcher() : QObject()
{
}

void Dispatcher::publish(QObject *sender, QEvent *event)
{
    QList<Subscriber *> subscribers = _subscribersByEventType.values(event->type());
    foreach(Subscriber *subscriber, subscribers)
    {
        if (!subscriber->sender || subscriber->sender == sender)
            QCoreApplication::sendEvent(subscriber->receiver, event);
    }

    delete event;
}


void Dispatcher::subscribe(QObject *receiver, QEvent::Type type, QObject *sender /* = NULL */)
{
    // subscribe to destroyed signal in order to remove
    // listener (receiver) from list of subscribers
    connect(receiver, SIGNAL(destroyed(QObject*)), this, SLOT(unsubscibe(QObject*)));

    // add subscriber
    _subscribersByEventType.insert(type, new Subscriber(receiver, sender));
}

void Dispatcher::unsubscibe(QObject *receiver)
{
    QMutableHashIterator<QEvent::Type, Subscriber *> i(_subscribersByEventType);

    while(i.hasNext()) {
        i.next();
        if (i.value()->receiver == receiver) {
            delete i.value();
            i.remove();
        }
    }
}