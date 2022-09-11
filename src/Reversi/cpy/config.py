class config(object):
    """! 
    Configuration file for training
    """
    #configuration-file for neural network
    num_iterations = 1
    num_games = 1
    num_mcts_sims = 50
    c_puct = 0.75
    l2_val = 0.0001
    momentum = 0.9
    learning_rate = 0.02
    t_policy_val = 0.0001
    temp_init = 1
    temp_final = 0.001
    temp_thresh = 10
    epochs = 10
    batch_size = 128
    dirichlet_alpha = 0.5
    epsilon = 0.25
    model_directory = "./models/"
    num_eval_games = 10
    eval_win_rate = 0.55
    load_model = 1
    human_play = 0
    resnet_blocks = 5
    record_loss = 1

    mcts_type = b"MCTS"
    eval_mcts_type = b"MCTS"
    online = 1
    ip = b"127.0.0.1"
    port = 7777
    use_MCTS_sims_4_train = 1
    max_MCTS_sims = 5
