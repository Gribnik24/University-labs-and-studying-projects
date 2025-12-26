#ifndef IPERSISTENT_H
#define IPERSISTENT_H

#include <iosfwd>

// Интерфейс персистентного объекта (умеет сохраняться и загружаться из потока)
class IPersistent {
public:
    virtual ~IPersistent() = default;

    // Запись состояния объекта в поток
    virtual void save(std::ostream& os) const = 0;

    // Чтение состояния объекта из потока
    virtual void load(std::istream& is) = 0;
};

#endif // IPERSISTENT_H
