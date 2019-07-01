#!/usr/bin/env python

import numpy as np
import tensorflow as tf
from sklearn.model_selection import train_test_split
from keras.layers import Input, LSTM, SimpleRNN, Dense, Softmax, Flatten
from keras.models import Model
from keras.optimizers import Adam
from sklearn.metrics import confusion_matrix

import acc_gyro_utils as agu

command_list = ['no_command',
                'right_flick',
                'up_flick',
                'left_flick',
                'down_flick',
                ]
#command_list = ['no_command',
#                'flick',
#                'rotate_clockwise',
#                'rotate_counter_clockwise',
#                ]

#training parameters
batch_size = 100
epochs = 200

#model definition
"""
LSTM MODEL =====================================================================
inputs = Input(shape=(None, 6))
x = LSTM(len(command_list), return_sequences=False, 
              activation='sigmoid')(inputs)
#x = LSTM(len(command_list), activation='tanh')(x)
#x = Dense(len(command_list), activation='sigmoid')(x)
outputs = Softmax()(x)
"""
n_lookback = 6
inputs = Input(shape=(n_lookback, 6))
x = Flatten()(inputs)
x = Dense(len(command_list), activation='linear')(x)
outputs = Softmax()(x)
model = Model(inputs, outputs)
model.compile(loss='categorical_crossentropy', optimizer=Adam(lr=1e-3), 
                metrics=['accuracy'])

command_dict_list = []
for i_command, command in enumerate(command_list):
    command_dict_list += agu.read_file(command + '.txt',i_command,
                                                        len(command_list))

train, test = train_test_split(command_dict_list, train_size=0.5)
while True:
    sub_list = [e for e in train if len(e['sequence'])==n_lookback]
    if len(sub_list)>0:
        seq_array = np.array([e['sequence'] for e in sub_list])
        com_oh_array = np.array([e['command_oh'] for e in sub_list])
        model.fit(seq_array, com_oh_array, 
                    batch_size=batch_size, epochs=epochs,
                    verbose=0)
        train_loss, train_acc = model.evaluate(seq_array, 
                                                com_oh_array, 
                                                batch_size=batch_size)
    
    sub_test = [e for e in test if e['seq_len']==n_lookback]
    if len(sub_test)>0:
        test_seq_array = np.array([e['sequence'] for e in sub_test])
        test_com_oh_array = np.array([e['command_oh'] for e in sub_test])
        test_loss, test_acc = model.evaluate(test_seq_array, 
                                             test_com_oh_array, 
                                             batch_size=batch_size)
        print 'test accuracy: {}'.format(test_acc)
        test_com_predict = model.predict(test_seq_array)
        print 'test confusion matrix:'
        print confusion_matrix(test_com_oh_array.argmax(axis=1),
                                test_com_predict.argmax(axis=1) )
        print 'train accuracy: {}'.format(train_acc)
        com_predict = model.predict(seq_array)
        print 'train confusion matrix:'
        print confusion_matrix(com_oh_array.argmax(axis=1),
                                com_predict.argmax(axis=1),)


    if test_acc > 0.98:
        model.save('latest_model.h5')
        break
