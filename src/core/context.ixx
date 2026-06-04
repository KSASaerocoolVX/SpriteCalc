module;

#include <string>
#include <unordered_map>
#include <utility>

export module core.context;

import core.exceptions;
import core.value;

export namespace core {

class Context {
public:
    // Добавление или обновление переменной
    void set(std::string name, Value value) {
        variables_[std::move(name)] = std::move(value);
    }

    // Удаление переменной. Если переменной нет, выбрасывает CalcError
    void erase(const std::string& name) {
        if (variables_.erase(name) == 0) {
            throw CalcError("unknown variable: " + name);
        }
    }

    // Очистка всех сохраненных переменных
    void clear() noexcept {
        variables_.clear();
    }

    // Проверка существования переменной
    [[nodiscard]] bool contains(const std::string& name) const {
        return variables_.contains(name);
    }

    // Получение значения переменной. Если переменной нет, выбрасывает CalcError
    [[nodiscard]] Value get(const std::string& name) const {
        const auto found = variables_.find(name);
        if (found == variables_.end()) {
            throw CalcError("unknown variable: " + name);
        }

        return found->second;
    }

    // Получение доступа ко всей карте переменных
    [[nodiscard]] const std::unordered_map<std::string, Value>& variables() const noexcept {
        return variables_;
    }

private:
    std::unordered_map<std::string, Value> variables_;
};

}