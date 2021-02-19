#include <math.h>
#include <stdlib.h>
#include "image.h"
#include "matrix.h"

// Run an activation function on each element in a matrix,
// modifies the matrix in place
// matrix m: Input to activation function
// ACTIVATION a: function to run
void activate_matrix(matrix m, ACTIVATION a)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        double sum = 0;
        for(j = 0; j < m.cols; ++j){
            double x = m.data[i][j];
            if(a == LOGISTIC){
                // TODO
                m.data[i][j] = (1.0)/(1.0 + exp(x * -1.0));
            } else if (a == RELU){
                // TODO
                m.data[i][j] = MAX(0, x); 
            } else if (a == LRELU){
                // TODO
                if (x < 0) {
                    m.data[i][j] = 0.1 * x;
                }
            } else if (a == SOFTMAX){
                // TODO
                m.data[i][j] = exp(x);
            }
            sum += m.data[i][j];
        }
        if (a == SOFTMAX) {
            // TODO: have to normalize by sum if we are using SOFTMAX
            for (j = 0; j < m.cols; j++) {
                m.data[i][j] /= sum;
            }
        }
    }
}

// Calculates the gradient of an activation function and multiplies it into
// the delta for a layer
// matrix m: an activated layer output
// ACTIVATION a: activation function for a layer
// matrix d: delta before activation gradient
void gradient_matrix(matrix m, ACTIVATION a, matrix d)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            double fx = m.data[i][j];
            // TODO: multiply the correct element of d by the gradient
            double grad = 1.0;
            if(a == LOGISTIC){
                // TODO
                grad = fx * (1.0 - fx);
            } else if (a == RELU){
                // TODO
                if (fx > 0) {
                    grad = 1.0;
                } else {
                    grad = 0;
                }
            } else if (a == LRELU){
                // TODO
                if (fx > 0) {
                    grad = 1.0;
                } else {
                    grad = 0.1;
                }
            }
            d.data[i][j] *= grad;
        }
    }
}

// Forward propagate information through a layer
// layer *l: pointer to the layer
// matrix in: input to layer
// returns: matrix that is output of the layer
matrix forward_layer(layer *l, matrix in)
{

    l->in = in;  // Save the input for backpropagation


    // TODO: fix this! multiply input by weights and apply activation function.
    matrix out = matrix_mult_matrix(in, l->w);
    activate_matrix(out, l->activation);

    free_matrix(l->out);// free the old output
    l->out = out;       // Save the current output for gradient calculation
    return out;
}

// Backward propagate derivatives through a layer
// layer *l: pointer to the layer
// matrix delta: partial derivative of loss w.r.t. output of layer
// returns: matrix, partial derivative of loss w.r.t. input to layer
matrix backward_layer(layer *l, matrix delta)
{
    // 1.4.1
    // delta is dL/dy
    // TODO: modify it in place to be dL/d(xw)
    gradient_matrix(l->out, l->activation, delta);

    // 1.4.2
    // TODO: then calculate dL/dw and save it in l->dw
    free_matrix(l->dw);
    // matrix dw = make_matrix(l->w.rows, l->w.cols); // replace this
    matrix xt = transpose_matrix(l->in);
    matrix dw = matrix_mult_matrix(xt, delta);
    l->dw = dw;
    free_matrix(xt);

    
    // 1.4.3
    // TODO: finally, calculate dL/dx and return it.
    // matrix dx = make_matrix(l->in.rows, l->in.cols); // replace this
    matrix wt = transpose_matrix(l->w);
    matrix dx = matrix_mult_matrix(delta, wt);
    free_matrix(wt);

    return dx;
}

// Update the weights at layer l
// layer *l: pointer to the layer
// double rate: learning rate
// double momentum: amount of momentum to use
// double decay: value for weight decay
void update_layer(layer *l, double rate, double momentum, double decay)
{
    // TODO:
    // Calculate Δw_t = dL/dw_t - λw_t + mΔw_{t-1}
    // save it to l->v
    matrix first_two = axpy_matrix(-1 * decay, l->w, l->dw);
    matrix dw_t = axpy_matrix(momentum, l->v, first_two);
    free_matrix(first_two);
    l->v = dw_t;

    // Update l->w
    l->w = axpy_matrix(rate, dw_t, l->w);

    // Remember to free any intermediate results to avoid memory leaks

}

// Make a new layer for our model
// int input: number of inputs to the layer
// int output: number of outputs from the layer
// ACTIVATION activation: the activation function to use
layer make_layer(int input, int output, ACTIVATION activation)
{
    layer l;
    l.in  = make_matrix(1,1);
    l.out = make_matrix(1,1);
    l.w   = random_matrix(input, output, sqrt(2./input));
    l.v   = make_matrix(input, output);
    l.dw  = make_matrix(input, output);
    l.activation = activation;
    return l;
}

// Run a model on input X
// model m: model to run
// matrix X: input to model
// returns: result matrix
matrix forward_model(model m, matrix X)
{
    int i;
    for(i = 0; i < m.n; ++i){
        X = forward_layer(m.layers + i, X);
    }
    return X;
}

// Run a model backward given gradient dL
// model m: model to run
// matrix dL: partial derivative of loss w.r.t. model output dL/dy
void backward_model(model m, matrix dL)
{
    matrix d = copy_matrix(dL);
    int i;
    for(i = m.n-1; i >= 0; --i){
        matrix prev = backward_layer(m.layers + i, d);
        free_matrix(d);
        d = prev;
    }
    free_matrix(d);
}

// Update the model weights
// model m: model to update
// double rate: learning rate
// double momentum: amount of momentum to use
// double decay: value for weight decay
void update_model(model m, double rate, double momentum, double decay)
{
    int i;
    for(i = 0; i < m.n; ++i){
        update_layer(m.layers + i, rate, momentum, decay);
    }
}

// Find the index of the maximum element in an array
// double *a: array
// int n: size of a, |a|
// returns: index of maximum element
int max_index(double *a, int n)
{
    if(n <= 0) return -1;
    int i;
    int max_i = 0;
    double max = a[0];
    for (i = 1; i < n; ++i) {
        if (a[i] > max){
            max = a[i];
            max_i = i;
        }
    }
    return max_i;
}

// Calculate the accuracy of a model on some data d
// model m: model to run
// data d: data to run on
// returns: accuracy, number correct / total
double accuracy_model(model m, data d)
{
    matrix p = forward_model(m, d.X);
    int i;
    int correct = 0;
    for(i = 0; i < d.y.rows; ++i){
        if(max_index(d.y.data[i], d.y.cols) == max_index(p.data[i], p.cols)) ++correct;
    }
    return (double)correct / d.y.rows;
}

// Calculate the cross-entropy loss for a set of predictions
// matrix y: the correct values
// matrix p: the predictions
// returns: average cross-entropy loss over data points, 1/n Σ(-ylog(p))
double cross_entropy_loss(matrix y, matrix p)
{
    int i, j;
    double sum = 0;
    for(i = 0; i < y.rows; ++i){
        for(j = 0; j < y.cols; ++j){
            sum += -y.data[i][j]*log(p.data[i][j]);
        }
    }
    return sum/y.rows;
}


// Train a model on a dataset using SGD
// model m: model to train
// data d: dataset to train on
// int batch: batch size for SGD
// int iters: number of iterations of SGD to run (i.e. how many batches)
// double rate: learning rate
// double momentum: momentum
// double decay: weight decay
void train_model(model m, data d, int batch, int iters, double rate, double momentum, double decay)
{
    int e;
    for(e = 0; e < iters; ++e){
        data b = random_batch(d, batch);
        matrix p = forward_model(m, b.X);
        fprintf(stderr, "%06d: Loss: %f\n", e, cross_entropy_loss(b.y, p));
        matrix dL = axpy_matrix(-1, p, b.y); // partial derivative of loss dL/dy
        backward_model(m, dL);
        update_model(m, rate/batch, momentum, decay);
        free_matrix(dL);
        free_data(b);
    }
}


// Questions 
//
// 5.2.2.1 Why might we be interested in both training accuracy and testing accuracy? What do these two numbers tell us about our current model?
// TODO
// The training accuracy will tell us how closely our model fits the training data. If this is very high, that MAY indicate that our model is trained thoroughly.
// The testing accuracy will tell us how well our model generalizes on data that it has never seen before. This is what we REALLY want to be high.
// If we find that training accuracy is very high, but testing accuracy is low, this indicates overfitting because our model fits the training data very well but
// if does not generalize well. From there we can update the model by increasing regularization or decreasing complexity in the design.
//
// 5.2.2.2 Try varying the model parameter for learning rate to different powers of 10 (i.e. 10^1, 10^0, 10^-1, 10^-2, 10^-3) and training the model. What patterns do you see and how does the choice of learning rate affect both the loss during training and the final model accuracy?
// TODO
// LR   |   Training Loss Observations  | Final Train Accuracy  |   Final Test Accuracy
// -----|-------------------------------|-----------------------|-----------------------
// 0.001|   loss decreasing steadily    |  0.8582               |   0.8679
// 0.01 |   loss decreasing steadily    |  0.9032               |   0.9089
// 0.1  |   loss decreasing steadily    |  0.9232               |   0.9209
// 1    |   loss bounces up and down    |  0.8891               |   0.8861
// 10   |   loss grows to nan           |  0.0987               |   0.098
// 
// Explanation:
// Our training stopping condition is based on number of iterations, not loss or weight convergence. So, learning rate is an especially important parameter.
// When the learning rate was 0.001 and 0.01 the loss was converging but the model trained less overall than the 0.1 learning rate because it was descending
// gradients slower (when LR was 0.001 and 0.01). When we increased LR to 1 and 10, we were descending gradients quickly, but we were overstepping due
// to high learning rate, which made the loss go up and down when LR was 1 and completely diverge when LR was 10.
//
// 5.2.2.3 Try varying the parameter for weight decay to different powers of 10: (10^0, 10^-1, 10^-2, 10^-3, 10^-4, 10^-5). How does weight decay affect the final model training and test accuracy?
// TODO
// Note: using LR = 0.1 from previous experiment
// Decay    | Final Train Accuracy  |   Final Test Accuracy
// ---------|-----------------------|-----------------------
// 0        |  0.9232               |   0.9209
// 0.00001  |  0.9232               |   0.9209
// 0.0001   |  0.9232               |   0.9209
// 0.001    |  0.9231               |   0.9209
// 0.01     |  0.9226               |   0.9207
// 0.1      |  0.9193               |   0.9196
// 1        |  0.8988               |   0.9049
//
// Explanation:
// As we increase weight decay, we see that we have decreasing training and testing accuracy. We have similar training and testing accuracy
// when weight decay is small (from 0 to 0.001). What I think this indicates is that this model does not need regularization, because biasing the
// model to be less complex with higher weight decay seems to decrease performance. Even with 0 weight decay we have the best accuracy. We might
// even benefit from increasing the complexity of the model.
//
// 5.2.3.1 Currently the model uses a logistic activation for the first layer. Try using a the different activation functions we programmed. How well do they perform? What's best?
// TODO
// Note: LR = 0.1 decay = 0.0
// Activation    | Final Train Accuracy  |   Final Test Accuracy
// --------------|-----------------------|-----------------------
// Logistic      |  0.9431               |   0.9409
// ReLU          |  0.9591               |   0.9508
// Leaky ReLU    |  0.9593               |   0.9516
// Softmax       |  0.0987               |   0.098
// Linear        |  0.8941               |   0.8878
//
// Explanation:
// The Leaky ReLU and ReLU activations performed the best. This is what I expected, as logistic and softmax are generally used for outputs, and linear is
// mainly used for the prediction at the end of the network.
//
// 5.2.3.2 Using the same activation, find the best (power of 10) learning rate for your model. What is the training accuracy and testing accuracy?
// TODO
// Note: using LRELU
// LR       | Final Train Accuracy  |   Final Test Accuracy
// ---------|-----------------------|-----------------------
// 0.001    |  0.6467               |   0.6545
// 0.01     |  0.8859               |   0.8936
// 0.1      |  0.9431               |   0.9409
// 1        |  0.9543               |   0.9503
// 10       |  0.2085               |   0.2105
//
// Explanation:
// The best learning rate for this network was 1. I was actually surprised that it was 1 instead of a power of 10 less than 1.
// We had faster loss convergence while increasing from 0.001 to 1, and the loss stayed high with LR 10.
//
// 5.2.3.3 Right now the regularization parameter `decay` is set to 0. Try adding some decay to your model. What happens, does it help? Why or why not may this be?
// TODO
// Note: LR = 1
// Decay    | Final Train Accuracy  |   Final Test Accuracy
// ---------|-----------------------|-----------------------
// 0        |  0.                   |   0.9209
// 0.00001  |  0.9232               |   0.9209
// 0.0001   |  0.9232               |   0.9209
// 0.001    |  0.9231               |   0.9209
// 0.01     |  0.9226               |   0.9207
// 0.1      |  0.9193               |   0.9196
// 1        |  0.8988               |   0.9049
//
// 5.2.3.4 Modify your model so it has 3 layers instead of two. The layers should be `inputs -> 64`, `64 -> 32`, and `32 -> outputs`. Also modify your model to train for 3000 iterations instead of 1000. Look at the training and testing error for different values of decay (powers of 10, 10^-4 -> 10^0). Which is best? Why?
// TODO
//
// 5.3.2.1 How well does your network perform on the CIFAR dataset?
// TODO
//



