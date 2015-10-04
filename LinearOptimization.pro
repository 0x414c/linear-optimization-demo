#-------------------------------------------------
#
# Project created by QtCreator 2015-09-18T20:28:30
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++14 warn_on

TARGET = LinearOptimization

TEMPLATE = app

SOURCES += src/main.cxx \
    lib/qcustomplot/qcustomplot.cpp \
    src/dantzignumericsolver.cxx \
    src/dataconvertors.cxx \
    src/ijsonserializable.cxx \
    src/inumericsolver.cxx \
    src/linearfunction.cxx \
    src/linearprogramdata.cxx \
    src/linearprogramdataserializable.cxx \
    src/mainwindow.cxx \
    src/numericvalidator.cxx \
    src/tablewidgetserializable.cxx \
    src/tablewidgetutils.cxx \
    src/tablewidgetutils.txx \
    src/utils.cxx \
    src/numericvalidatingdelegate.txx \
    src/numericvalidatingdelegate.cxx

HEADERS  += src/mainwindow.hxx \
    lib/qcustomplot/qcustomplot.h \
    src/dantzignumericsolver.hxx \
    src/dataconvertors.hxx \
    src/ijsonserializable.hxx \
    src/inumericsolver.hxx \
    src/linearfunction.hxx \
    src/linearprogramdata.hxx \
    src/linearprogramdataserializable.hxx \
    src/numericvalidator.hxx \
    src/tablewidgetserializable.hxx \
    src/tablewidgetutils.hxx \
    src/utils.hxx \
    src/numericvalidatingdelegate.hxx

#    lib/eigen3/Eigen/src/Cholesky/LDLT.h \
#    lib/eigen3/Eigen/src/Cholesky/LLT.h \
#    lib/eigen3/Eigen/src/Cholesky/LLT_MKL.h \
#    lib/eigen3/Eigen/src/CholmodSupport/CholmodSupport.h \
#    lib/eigen3/Eigen/src/Core/arch/AltiVec/Complex.h \
#    lib/eigen3/Eigen/src/Core/arch/AltiVec/PacketMath.h \
#    lib/eigen3/Eigen/src/Core/arch/Default/Settings.h \
#    lib/eigen3/Eigen/src/Core/arch/NEON/Complex.h \
#    lib/eigen3/Eigen/src/Core/arch/NEON/PacketMath.h \
#    lib/eigen3/Eigen/src/Core/arch/SSE/Complex.h \
#    lib/eigen3/Eigen/src/Core/arch/SSE/MathFunctions.h \
#    lib/eigen3/Eigen/src/Core/arch/SSE/PacketMath.h \
#    lib/eigen3/Eigen/src/Core/products/CoeffBasedProduct.h \
#    lib/eigen3/Eigen/src/Core/products/GeneralBlockPanelKernel.h \
#    lib/eigen3/Eigen/src/Core/products/GeneralMatrixMatrix.h \
#    lib/eigen3/Eigen/src/Core/products/GeneralMatrixMatrix_MKL.h \
#    lib/eigen3/Eigen/src/Core/products/GeneralMatrixMatrixTriangular.h \
#    lib/eigen3/Eigen/src/Core/products/GeneralMatrixMatrixTriangular_MKL.h \
#    lib/eigen3/Eigen/src/Core/products/GeneralMatrixVector.h \
#    lib/eigen3/Eigen/src/Core/products/GeneralMatrixVector_MKL.h \
#    lib/eigen3/Eigen/src/Core/products/Parallelizer.h \
#    lib/eigen3/Eigen/src/Core/products/SelfadjointMatrixMatrix.h \
#    lib/eigen3/Eigen/src/Core/products/SelfadjointMatrixMatrix_MKL.h \
#    lib/eigen3/Eigen/src/Core/products/SelfadjointMatrixVector.h \
#    lib/eigen3/Eigen/src/Core/products/SelfadjointMatrixVector_MKL.h \
#    lib/eigen3/Eigen/src/Core/products/SelfadjointProduct.h \
#    lib/eigen3/Eigen/src/Core/products/SelfadjointRank2Update.h \
#    lib/eigen3/Eigen/src/Core/products/TriangularMatrixMatrix.h \
#    lib/eigen3/Eigen/src/Core/products/TriangularMatrixMatrix_MKL.h \
#    lib/eigen3/Eigen/src/Core/products/TriangularMatrixVector.h \
#    lib/eigen3/Eigen/src/Core/products/TriangularMatrixVector_MKL.h \
#    lib/eigen3/Eigen/src/Core/products/TriangularSolverMatrix.h \
#    lib/eigen3/Eigen/src/Core/products/TriangularSolverMatrix_MKL.h \
#    lib/eigen3/Eigen/src/Core/products/TriangularSolverVector.h \
#    lib/eigen3/Eigen/src/Core/util/BlasUtil.h \
#    lib/eigen3/Eigen/src/Core/util/Constants.h \
#    lib/eigen3/Eigen/src/Core/util/DisableStupidWarnings.h \
#    lib/eigen3/Eigen/src/Core/util/ForwardDeclarations.h \
#    lib/eigen3/Eigen/src/Core/util/Macros.h \
#    lib/eigen3/Eigen/src/Core/util/Memory.h \
#    lib/eigen3/Eigen/src/Core/util/Meta.h \
#    lib/eigen3/Eigen/src/Core/util/MKL_support.h \
#    lib/eigen3/Eigen/src/Core/util/NonMPL2.h \
#    lib/eigen3/Eigen/src/Core/util/ReenableStupidWarnings.h \
#    lib/eigen3/Eigen/src/Core/util/StaticAssert.h \
#    lib/eigen3/Eigen/src/Core/util/XprHelper.h \
#    lib/eigen3/Eigen/src/Core/Array.h \
#    lib/eigen3/Eigen/src/Core/ArrayBase.h \
#    lib/eigen3/Eigen/src/Core/ArrayWrapper.h \
#    lib/eigen3/Eigen/src/Core/Assign.h \
#    lib/eigen3/Eigen/src/Core/Assign_MKL.h \
#    lib/eigen3/Eigen/src/Core/BandMatrix.h \
#    lib/eigen3/Eigen/src/Core/Block.h \
#    lib/eigen3/Eigen/src/Core/BooleanRedux.h \
#    lib/eigen3/Eigen/src/Core/CommaInitializer.h \
#    lib/eigen3/Eigen/src/Core/CoreIterators.h \
#    lib/eigen3/Eigen/src/Core/CwiseBinaryOp.h \
#    lib/eigen3/Eigen/src/Core/CwiseNullaryOp.h \
#    lib/eigen3/Eigen/src/Core/CwiseUnaryOp.h \
#    lib/eigen3/Eigen/src/Core/CwiseUnaryView.h \
#    lib/eigen3/Eigen/src/Core/DenseBase.h \
#    lib/eigen3/Eigen/src/Core/DenseCoeffsBase.h \
#    lib/eigen3/Eigen/src/Core/DenseStorage.h \
#    lib/eigen3/Eigen/src/Core/Diagonal.h \
#    lib/eigen3/Eigen/src/Core/DiagonalMatrix.h \
#    lib/eigen3/Eigen/src/Core/DiagonalProduct.h \
#    lib/eigen3/Eigen/src/Core/Dot.h \
#    lib/eigen3/Eigen/src/Core/EigenBase.h \
#    lib/eigen3/Eigen/src/Core/Flagged.h \
#    lib/eigen3/Eigen/src/Core/ForceAlignedAccess.h \
#    lib/eigen3/Eigen/src/Core/Functors.h \
#    lib/eigen3/Eigen/src/Core/Fuzzy.h \
#    lib/eigen3/Eigen/src/Core/GeneralProduct.h \
#    lib/eigen3/Eigen/src/Core/GenericPacketMath.h \
#    lib/eigen3/Eigen/src/Core/GlobalFunctions.h \
#    lib/eigen3/Eigen/src/Core/IO.h \
#    lib/eigen3/Eigen/src/Core/Map.h \
#    lib/eigen3/Eigen/src/Core/MapBase.h \
#    lib/eigen3/Eigen/src/Core/MathFunctions.h \
#    lib/eigen3/Eigen/src/Core/Matrix.h \
#    lib/eigen3/Eigen/src/Core/MatrixBase.h \
#    lib/eigen3/Eigen/src/Core/NestByValue.h \
#    lib/eigen3/Eigen/src/Core/NoAlias.h \
#    lib/eigen3/Eigen/src/Core/NumTraits.h \
#    lib/eigen3/Eigen/src/Core/PermutationMatrix.h \
#    lib/eigen3/Eigen/src/Core/PlainObjectBase.h \
#    lib/eigen3/Eigen/src/Core/ProductBase.h \
#    lib/eigen3/Eigen/src/Core/Random.h \
#    lib/eigen3/Eigen/src/Core/Redux.h \
#    lib/eigen3/Eigen/src/Core/Ref.h \
#    lib/eigen3/Eigen/src/Core/Replicate.h \
#    lib/eigen3/Eigen/src/Core/ReturnByValue.h \
#    lib/eigen3/Eigen/src/Core/Reverse.h \
#    lib/eigen3/Eigen/src/Core/Select.h \
#    lib/eigen3/Eigen/src/Core/SelfAdjointView.h \
#    lib/eigen3/Eigen/src/Core/SelfCwiseBinaryOp.h \
#    lib/eigen3/Eigen/src/Core/SolveTriangular.h \
#    lib/eigen3/Eigen/src/Core/StableNorm.h \
#    lib/eigen3/Eigen/src/Core/Stride.h \
#    lib/eigen3/Eigen/src/Core/Swap.h \
#    lib/eigen3/Eigen/src/Core/Transpose.h \
#    lib/eigen3/Eigen/src/Core/Transpositions.h \
#    lib/eigen3/Eigen/src/Core/TriangularMatrix.h \
#    lib/eigen3/Eigen/src/Core/VectorBlock.h \
#    lib/eigen3/Eigen/src/Core/VectorwiseOp.h \
#    lib/eigen3/Eigen/src/Core/Visitor.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/AlignedBox.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/All.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/AngleAxis.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/Hyperplane.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/ParametrizedLine.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/Quaternion.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/Rotation2D.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/RotationBase.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/Scaling.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/Transform.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Geometry/Translation.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Block.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Cwise.h \
#    lib/eigen3/Eigen/src/Eigen2Support/CwiseOperators.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Lazy.h \
#    lib/eigen3/Eigen/src/Eigen2Support/LeastSquares.h \
#    lib/eigen3/Eigen/src/Eigen2Support/LU.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Macros.h \
#    lib/eigen3/Eigen/src/Eigen2Support/MathFunctions.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Memory.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Meta.h \
#    lib/eigen3/Eigen/src/Eigen2Support/Minor.h \
#    lib/eigen3/Eigen/src/Eigen2Support/QR.h \
#    lib/eigen3/Eigen/src/Eigen2Support/SVD.h \
#    lib/eigen3/Eigen/src/Eigen2Support/TriangularSolver.h \
#    lib/eigen3/Eigen/src/Eigen2Support/VectorBlock.h \
#    lib/eigen3/Eigen/src/Eigenvalues/ComplexEigenSolver.h \
#    lib/eigen3/Eigen/src/Eigenvalues/ComplexSchur.h \
#    lib/eigen3/Eigen/src/Eigenvalues/ComplexSchur_MKL.h \
#    lib/eigen3/Eigen/src/Eigenvalues/EigenSolver.h \
#    lib/eigen3/Eigen/src/Eigenvalues/GeneralizedEigenSolver.h \
#    lib/eigen3/Eigen/src/Eigenvalues/GeneralizedSelfAdjointEigenSolver.h \
#    lib/eigen3/Eigen/src/Eigenvalues/HessenbergDecomposition.h \
#    lib/eigen3/Eigen/src/Eigenvalues/MatrixBaseEigenvalues.h \
#    lib/eigen3/Eigen/src/Eigenvalues/RealQZ.h \
#    lib/eigen3/Eigen/src/Eigenvalues/RealSchur.h \
#    lib/eigen3/Eigen/src/Eigenvalues/RealSchur_MKL.h \
#    lib/eigen3/Eigen/src/Eigenvalues/SelfAdjointEigenSolver.h \
#    lib/eigen3/Eigen/src/Eigenvalues/SelfAdjointEigenSolver_MKL.h \
#    lib/eigen3/Eigen/src/Eigenvalues/Tridiagonalization.h \
#    lib/eigen3/Eigen/src/Geometry/arch/Geometry_SSE.h \
#    lib/eigen3/Eigen/src/Geometry/AlignedBox.h \
#    lib/eigen3/Eigen/src/Geometry/AngleAxis.h \
#    lib/eigen3/Eigen/src/Geometry/EulerAngles.h \
#    lib/eigen3/Eigen/src/Geometry/Homogeneous.h \
#    lib/eigen3/Eigen/src/Geometry/Hyperplane.h \
#    lib/eigen3/Eigen/src/Geometry/OrthoMethods.h \
#    lib/eigen3/Eigen/src/Geometry/ParametrizedLine.h \
#    lib/eigen3/Eigen/src/Geometry/Quaternion.h \
#    lib/eigen3/Eigen/src/Geometry/Rotation2D.h \
#    lib/eigen3/Eigen/src/Geometry/RotationBase.h \
#    lib/eigen3/Eigen/src/Geometry/Scaling.h \
#    lib/eigen3/Eigen/src/Geometry/Transform.h \
#    lib/eigen3/Eigen/src/Geometry/Translation.h \
#    lib/eigen3/Eigen/src/Geometry/Umeyama.h \
#    lib/eigen3/Eigen/src/Householder/BlockHouseholder.h \
#    lib/eigen3/Eigen/src/Householder/Householder.h \
#    lib/eigen3/Eigen/src/Householder/HouseholderSequence.h \
#    lib/eigen3/Eigen/src/IterativeLinearSolvers/BasicPreconditioners.h \
#    lib/eigen3/Eigen/src/IterativeLinearSolvers/BiCGSTAB.h \
#    lib/eigen3/Eigen/src/IterativeLinearSolvers/ConjugateGradient.h \
#    lib/eigen3/Eigen/src/IterativeLinearSolvers/IncompleteLUT.h \
#    lib/eigen3/Eigen/src/IterativeLinearSolvers/IterativeSolverBase.h \
#    lib/eigen3/Eigen/src/Jacobi/Jacobi.h \
#    lib/eigen3/Eigen/src/LU/arch/Inverse_SSE.h \
#    lib/eigen3/Eigen/src/LU/Determinant.h \
#    lib/eigen3/Eigen/src/LU/FullPivLU.h \
#    lib/eigen3/Eigen/src/LU/Inverse.h \
#    lib/eigen3/Eigen/src/LU/PartialPivLU.h \
#    lib/eigen3/Eigen/src/LU/PartialPivLU_MKL.h \
#    lib/eigen3/Eigen/src/MetisSupport/MetisSupport.h \
#    lib/eigen3/Eigen/src/misc/blas.h \
#    lib/eigen3/Eigen/src/misc/Image.h \
#    lib/eigen3/Eigen/src/misc/Kernel.h \
#    lib/eigen3/Eigen/src/misc/Solve.h \
#    lib/eigen3/Eigen/src/misc/SparseSolve.h \
#    lib/eigen3/Eigen/src/OrderingMethods/Amd.h \
#    lib/eigen3/Eigen/src/OrderingMethods/Eigen_Colamd.h \
#    lib/eigen3/Eigen/src/OrderingMethods/Ordering.h \
#    lib/eigen3/Eigen/src/PardisoSupport/PardisoSupport.h \
#    lib/eigen3/Eigen/src/PaStiXSupport/PaStiXSupport.h \
#    lib/eigen3/Eigen/src/plugins/ArrayCwiseBinaryOps.h \
#    lib/eigen3/Eigen/src/plugins/ArrayCwiseUnaryOps.h \
#    lib/eigen3/Eigen/src/plugins/BlockMethods.h \
#    lib/eigen3/Eigen/src/plugins/CommonCwiseBinaryOps.h \
#    lib/eigen3/Eigen/src/plugins/CommonCwiseUnaryOps.h \
#    lib/eigen3/Eigen/src/plugins/MatrixCwiseBinaryOps.h \
#    lib/eigen3/Eigen/src/plugins/MatrixCwiseUnaryOps.h \
#    lib/eigen3/Eigen/src/QR/ColPivHouseholderQR.h \
#    lib/eigen3/Eigen/src/QR/ColPivHouseholderQR_MKL.h \
#    lib/eigen3/Eigen/src/QR/FullPivHouseholderQR.h \
#    lib/eigen3/Eigen/src/QR/HouseholderQR.h \
#    lib/eigen3/Eigen/src/QR/HouseholderQR_MKL.h \
#    lib/eigen3/Eigen/src/SparseCholesky/SimplicialCholesky.h \
#    lib/eigen3/Eigen/src/SparseCholesky/SimplicialCholesky_impl.h \
#    lib/eigen3/Eigen/src/SparseCore/AmbiVector.h \
#    lib/eigen3/Eigen/src/SparseCore/CompressedStorage.h \
#    lib/eigen3/Eigen/src/SparseCore/ConservativeSparseSparseProduct.h \
#    lib/eigen3/Eigen/src/SparseCore/MappedSparseMatrix.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseBlock.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseColEtree.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseCwiseBinaryOp.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseCwiseUnaryOp.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseDenseProduct.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseDiagonalProduct.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseDot.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseFuzzy.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseMatrix.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseMatrixBase.h \
#    lib/eigen3/Eigen/src/SparseCore/SparsePermutation.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseProduct.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseRedux.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseSelfAdjointView.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseSparseProductWithPruning.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseTranspose.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseTriangularView.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseUtil.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseVector.h \
#    lib/eigen3/Eigen/src/SparseCore/SparseView.h \
#    lib/eigen3/Eigen/src/SparseCore/TriangularSolver.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_column_bmod.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_column_dfs.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_copy_to_ucol.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_gemm_kernel.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_heap_relax_snode.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_kernel_bmod.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_Memory.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_panel_bmod.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_panel_dfs.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_pivotL.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_pruneL.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_relax_snode.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_Structs.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_SupernodalMatrix.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLU_Utils.h \
#    lib/eigen3/Eigen/src/SparseLU/SparseLUImpl.h \
#    lib/eigen3/Eigen/src/SparseQR/SparseQR.h \
#    lib/eigen3/Eigen/src/SPQRSupport/SuiteSparseQRSupport.h \
#    lib/eigen3/Eigen/src/StlSupport/details.h \
#    lib/eigen3/Eigen/src/StlSupport/StdDeque.h \
#    lib/eigen3/Eigen/src/StlSupport/StdList.h \
#    lib/eigen3/Eigen/src/StlSupport/StdVector.h \
#    lib/eigen3/Eigen/src/SuperLUSupport/SuperLUSupport.h \
#    lib/eigen3/Eigen/src/SVD/JacobiSVD.h \
#    lib/eigen3/Eigen/src/SVD/JacobiSVD_MKL.h \
#    lib/eigen3/Eigen/src/SVD/UpperBidiagonalization.h \
#    lib/eigen3/Eigen/src/UmfPackSupport/UmfPackSupport.h \
#    lib/boost/detail/workaround.hpp \
#    lib/boost/integer/common_factor_rt.hpp \
#    lib/boost/assert.hpp \
#    lib/boost/call_traits.hpp \
#    lib/boost/operators.hpp \
#    lib/boost/rational.hpp \
#    lib/boost/static_assert.hpp

FORMS += forms/mainwindow.ui

RESOURCES += res/resources.qrc
