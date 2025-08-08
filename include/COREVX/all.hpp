// CoreVX single-include header for C++ development
// Exposes the existing C++ classes under the corevx namespace without refactoring
// and re-exports the stable OpenVX C API for convenience.

#pragma once

// Public OpenVX C API (stable surface)
#include <VX/vx.h>
#include <VX/vx_api.h>

// C++ class headers (implementation classes). These expose full class methods.
// Note: These are part of the current implementation and may include internal details.
// They are intentionally exposed here to allow direct C++ usage of the pipeline engine.
#include "framework/include/vx_internal.h"
#include "framework/include/vx_reference.h"
#include "framework/include/vx_context.h"
#include "framework/include/vx_graph.h"
#include "framework/include/vx_node.h"
#include "framework/include/vx_image.h"
#include "framework/include/vx_array.h"
#include "framework/include/vx_tensor.h"
#include "framework/include/vx_matrix.h"
#include "framework/include/vx_threshold.h"
#include "framework/include/vx_distribution.h"
#include "framework/include/vx_convolution.h"
#include "framework/include/vx_pyramid.h"
#include "framework/include/vx_remap.h"
#include "framework/include/vx_object_array.h"
#include "framework/include/vx_parameter.h"
#include "framework/include/vx_kernel.h"
#include "framework/include/vx_meta_format.h"
#include "framework/include/vx_import.h"
#include "framework/include/vx_error.h"
#include "framework/include/vx_delay.h"
#include "framework/include/vx_event_queue.hpp"
