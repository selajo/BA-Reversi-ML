import os
import tensorflow as tf
import numpy as np
from datetime import datetime
from config import config
from tensorflow import keras
import tensorboard
import sys
from random import shuffle

"""

Model and creation based on: https://github.com/blanyal/alpha-zero/blob/master/neural_net.py

"""

class model(object):
    """!
    Holds model for neural network
    """
    def __init__(self):
        """!
        Creates resnet for alphazero
        """
        tf.compat.v1.logging.set_verbosity(tf.compat.v1.logging.ERROR)
        #tf.compat.v1.enable_eager_execution()
        self.row = 8
        self.action_size = 64
        self.pi = None
        self.v = None
        
        self.graph = tf.Graph()
        with self.graph.as_default():
            self.states = tf.compat.v1.placeholder(tf.float32, shape=[None, self.row, self.row])
            self.training = tf.compat.v1.placeholder(tf.bool)

            #Input layer
            input_layer = tf.reshape(self.states, [-1, self.row, self.row, 1])

            #convolutional block
            conv1 = tf.compat.v1.layers.conv2d(
                inputs=input_layer,
                filters=256,
                kernel_size=[3,3],
                padding="same",
                strides=1
            )

            batch_norm1 = tf.compat.v1.layers.batch_normalization(
                inputs=conv1,
                training=self.training
            )

            relu1 = tf.nn.relu(batch_norm1)

            resnet_in_out = relu1
            
            for i in range(config.resnet_blocks):
                #Residual Block
                conv2 = tf.compat.v1.layers.conv2d(
                    inputs=resnet_in_out,
                    filters=256,
                    kernel_size=[3,3],
                    padding="same",
                    strides=1
                )

                batch_norm2 = tf.compat.v1.layers.batch_normalization(
                    inputs=conv2,
                    training=self.training
                )

                relu2 = tf.nn.relu(batch_norm2)

                conv3 = tf.compat.v1.layers.conv2d(
                    inputs=relu2,
                    filters=256,
                    kernel_size=[3,3],
                    padding="same",
                    strides=1
                )

                batch_norm3 = tf.compat.v1.layers.batch_normalization(
                    inputs=conv3,
                    training=self.training
                )

                resnet_skip = tf.add(batch_norm3, resnet_in_out)

                resnet_in_out = tf.nn.relu(resnet_skip)
            

            # Policy Head
            conv4 = tf.compat.v1.layers.conv2d(
                inputs=resnet_in_out,
                filters=2,
                kernel_size=[1,1],
                padding="same",
                strides=1
            )

            batch_norm4 = tf.compat.v1.layers.batch_normalization(
                inputs=conv4,
                training=self.training
            )

            relu4 = tf.nn.relu(batch_norm4)

            relu4_flat = tf.reshape(relu4, [-1, self.row * self.row * 2])

            logits = tf.compat.v1.layers.dense(inputs=relu4_flat, units=self.action_size)

            self.pi = tf.nn.softmax(logits)

             # Value Head
            conv5 = tf.compat.v1.layers.conv2d(
                inputs=resnet_in_out,
                filters=1,
                kernel_size=[1, 1],
                padding="same",
                strides=1)

            batch_norm5 = tf.compat.v1.layers.batch_normalization(
                inputs=conv5,
                training=self.training
            )

            relu5 = tf.nn.relu(batch_norm5)

            relu5_flat = tf.reshape(relu5, [-1, self.action_size])

            dense1 = tf.compat.v1.layers.dense(inputs=relu5_flat,
                                     units=256)

            relu6 = tf.nn.relu(dense1)

            dense2 = tf.compat.v1.layers.dense(inputs=relu6,
                                     units=1)

            self.v = tf.nn.tanh(dense2)

            # Loss Function
            self.train_pis = tf.compat.v1.placeholder(tf.float32,
                                            shape=[None, self.action_size])
            self.train_vs = tf.compat.v1.placeholder(tf.float32, shape=[None])

            self.loss_pi = tf.compat.v1.losses.softmax_cross_entropy(self.train_pis,
                                                           self.pi)
            self.loss_v = tf.losses.mean_squared_error(self.train_vs,
                                                       tf.reshape(self.v,
                                                                  shape=[-1, ]))
            self.total_loss = self.loss_pi + self.loss_v

            # Stochastic gradient descent with momentum
            # global_step = tf.Variable(0, trainable=False)

            # learning_rate = tf.train.exponential_decay(CFG.learning_rate,
            #                                            global_step,
            #                                            20000,
            #                                            0.96,
            #                                            staircase=True)

            optimizer = tf.compat.v1.train.MomentumOptimizer(
                learning_rate=config.learning_rate,
                momentum=config.momentum,
                use_nesterov=False)

            self.train_op = optimizer.minimize(self.total_loss)

            # Create a saver for writing training checkpoints.
            self.saver = tf.compat.v1.train.Saver()

            # Create a session for running Ops on the Graph.
            self.sess = tf.compat.v1.Session()

            # Initialize the session.
            self.sess.run(tf.compat.v1.global_variables_initializer())


class ModelWrapper(object):
    """!
    Warps model
    """

    def __init__(self):
        """!
        Created model and session
        """
        #self.game = game
        self.net = model()
        self.sess = self.net.sess
        self.loss_file = "loss_" + sys.argv[2] + ".txt"

    def predict(self, state):
        """!
        Predicts pi and v by given state
        @param state current game board
        """
        # predicts move propabilities
        state = state[np.newaxis, :, :]

        pi, v = self.sess.run([self.net.pi, self.net.v],
                        feed_dict={self.net.states: state,
                                    self.net.training: False})
        
        return pi[0], v[0][0]
       
    def train2(self, training_data):
        
        shuffle(training_data)
        
        examples_num = len(training_data)

        #Divide epoch into batches
        for i in range(0, examples_num, config.batch_size):
            states, pis, vs = map(list, 
                                        zip(*training_data[i:i + config.batch_size]))
            feed_dict = {self.net.states: states,
                         self.net.train_pis: pis,
                         self.net.train_vs: vs,
                         self.net.training: True}
                
            self.sess.run(self.net.train_op, feed_dict=feed_dict)

            #writer = tf.compat.v1.summary.FileWriter("output", self.sess.graph)
            pi_loss, v_loss = self.sess.run(
                [self.net.loss_pi, self.net.loss_v],
                feed_dict=feed_dict
            )
            #writer.add_summary(result, 0)
            #writer.close()

            if config.record_loss:
                # create dir if it doesn't exist
                if not os.path.exists(config.model_directory):
                    os.mkdir(config.model_directory)
                file_path = config.model_directory + self.loss_file

                with open(file_path, 'a') as loss_file:
                    loss_file.write('%f|%f\n' % (pi_loss, v_loss))

    def train(self, training_data):
        """!
        Trains neural network
        @param training_data Consists of game boards, train pis, trains vs
        """
        print("\nTraining the network.\n")

        for epoch in range(config.epochs):
            print("Epoch", epoch + 1)

            examples_num = len(training_data)

            #Divide epoch into batches
            for i in range(0, examples_num, config.batch_size):
                states, pis, vs = map(list, 
                                        zip(*training_data[i:i + config.batch_size]))
                feed_dict = {self.net.states: states,
                             self.net.train_pis: pis,
                             self.net.train_vs: vs,
                             self.net.training: True}
                
                self.sess.run(self.net.train_op, feed_dict=feed_dict)

                pi_loss, v_loss = self.sess.run(
                    [self.net.loss_pi, self.net.loss_v],
                    feed_dict=feed_dict
                )

                if config.record_loss:
                    # create dir if it doesn't exist
                    if not os.path.exists(config.model_directory):
                        os.mkdir(config.model_directory)
                    file_path = config.model_directory + self.loss_file

                    with open(file_path, 'a') as loss_file:
                        loss_file.write('%f|%f\n' % (pi_loss, v_loss))
        print("\n")

    def save_model(self, filename="current_model"):
        """!
        Saves model to disk
        @param filename Filename
        """
        if not os.path.exists(config.model_directory):
            os.mkdir(config.model_directory)
        
        file_path = config.model_directory + filename

        print("Saving model:", filename, "at", config.model_directory)
        tf.compat.v1.disable_v2_behavior()
        self.net.saver.save(self.sess, file_path)
    
    def load_model(self, filename="current_model"):
        """!
        Saves model from disk
        @param filename Filename
        """
        file_path = config.model_directory + filename

        print("Loading model:", filename, "from", config.model_directory)
        tf.compat.v1.disable_v2_behavior()
        self.net.saver.restore(self.sess, file_path)
