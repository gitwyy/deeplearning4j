//
// Created by raver119 on 02.11.2017.
//

#include <op_boilerplate.h>
#if NOT_EXCLUDED(OP_expand_dims)

#include <ops/declarable/CustomOperations.h>

namespace nd4j {
    namespace ops {
        CUSTOM_OP_IMPL(expand_dims, 1, 1, false, 0, 1) {
            auto input = INPUT_VARIABLE(0);
            auto output = OUTPUT_VARIABLE(0);

            if (input->isScalar()) {
                output->assign(input->getScalar(0));
                return ND4J_STATUS_OK;
            }

            auto axis = INT_ARG(0);

            if (axis < 0)
                axis += input->rankOf() + 1;


            REQUIRE_TRUE(axis >= 0 && axis <= input->rankOf()+1, 0, "ExpandDims: axis should be in range of 0...%i in this case, but got %i instead", input->rankOf() + 1, axis);

            std::vector<Nd4jLong> shape;
            for(int e = 0; e < input->rankOf(); e++)
                shape.emplace_back(input->sizeAt(e));

            shape.insert(shape.begin() + axis, 1);

            auto tmp = input->reshape(input->ordering(), shape);
            output->assign(tmp);

            delete tmp;

            STORE_RESULT(output);

            return ND4J_STATUS_OK;
        }
        DECLARE_SHAPE_FN(expand_dims) {
            auto inShape = inputShape->at(0);

            // 0D scalar edge case
            if (shape::rank(inShape) == 0) {
                Nd4jLong* newShape;
                ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(1), Nd4jLong);

                Nd4jLong x = 1;
                shape::shapeBuffer(1, &x, newShape);
                return SHAPELIST(newShape);
            }

            // FIXME: temp workaround for TF
            if (shape::isScalar(inShape)) {
                Nd4jLong* newShape;
                ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(inShape), Nd4jLong);

                shape::shapeBuffer(2, shape::shapeOf(inShape), newShape);
                return SHAPELIST(newShape);
            }

            auto x_rank = shape::rank(inShape);
            char order = shape::order(inShape);

            auto axis = INT_ARG(0);

            if (axis < 0)
                axis += x_rank + 1;

            Nd4jLong* newShape;
            ALLOCATE(newShape, block.getWorkspace(), shape::shapeInfoLength(x_rank+1), Nd4jLong);

            std::vector<Nd4jLong> shape;
            for(int e = 0; e < x_rank; e++)
                shape.emplace_back(shape::shapeOf(inShape)[e]);

            shape.insert(shape.begin() + axis, 1);

            if (order == 'c')
                shape::shapeBuffer(x_rank+1, shape.data(), newShape);
            else
                shape::shapeBufferFortran(x_rank+1, shape.data(), newShape);


            return SHAPELIST(newShape);
        }
    }
}

#endif