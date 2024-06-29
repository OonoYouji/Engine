#include <BaseCollider.h>

void BaseCollider::SetName(const std::string& name) {
	name_ = name;
}

const std::string& BaseCollider::GetName() const {
	return name_;
}
