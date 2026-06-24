#pragma once

#include "data/dto/HealthDto.hpp"
#include "domain/entities/HealthStatus.hpp"

namespace HealthMapper {

HealthStatus toDomain(const HealthDto &dto);

} // namespace HealthMapper
