#include "nav.hpp"

bool nv::put(const QString &path) {
    if (path.isEmpty() || (at >= 0 && log.value(at) == path))
        return false;

    while (log.size() > at + 1)
        log.removeLast();

    log.append(path);
    at = log.size() - 1;
    return true;
}

QString nv::bk() {
    if (at <= 0)
        return {};

    return log.at(--at);
}

QString nv::fw() {
    if (at + 1 >= log.size())
        return {};

    return log.at(++at);
}

void nv::clr() {
    log.clear();
    at = -1;
}