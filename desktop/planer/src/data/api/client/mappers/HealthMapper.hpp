#pragma once

#include "data/api/client/dto/HealthDto.hpp"
#include "domain/entities/HealthStatus.hpp"

namespace HealthMapper {

HealthStatus toDomain(const HealthDto &dto);

} // namespace HealthMapper
