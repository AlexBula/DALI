// Copyright (c) 2017-2020, NVIDIA CORPORATION. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <tuple>
#include "dali/operators/generic/one_hot.h"
#include "dali/kernels/kernel_params.h"
#include "dali/kernels/test/kernel_test_utils.h"
#include "utility"
#include "dali/test/tensor_test_utils.h"

namespace dali {
namespace testing {

template <class InputOutputTypes>
class OneHotOpTest : public ::testing::Test {
  using In = typename InputOutputTypes::In;
  using Out = typename InputOutputTypes::Out;

 protected:
    void SetUp() final {
        output_.resize(nclasses_, 0);
    }

    int buffer_length_ = 10;
    int nclasses_ = 20;
    std::vector<In> input_{1, 3, 5, 0, 1, 2, 10, 15, 7, 6};
    std::vector<Out> output_;
    TensorShape<1> input_shape_ = {1};
    TensorShape<1> output_shape_ = {nclasses_};
};

using TestTypes = std::tuple<uint8_t, int8_t, uint16_t, int16_t, int32_t, int64_t, float>;
INPUT_OUTPUT_TYPED_TEST_SUITE(OneHotOpTest, TestTypes);

TYPED_TEST(OneHotOpTest, TestVariousTypes) {
    using In = typename TypeParam::In;
    using Out = typename TypeParam::Out;
    for (auto it = this->input_.begin(); it != this->input_.end(); ++it) {
        auto input = make_tensor_cpu(reinterpret_cast<const In*>(&(*it)),
                                     this->input_shape_);
        auto output = make_tensor_cpu(reinterpret_cast<Out*>(this->output_.data()),
                                      this->output_shape_);
        dali::detail::DoOneHot(output, input, 20, 1, 0);
        auto ptr = output.data;
        for (int i = 0; i < this->nclasses_; ++i) {
            if (i == *it) {
                ASSERT_EQ(ptr[i], 1);
            } else {
                ASSERT_EQ(ptr[i], 0);
            }
        }
    }
}

}  // namespace testing
}  // namespace dali
