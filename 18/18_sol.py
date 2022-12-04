import ast
import os
import copy
import sys

def read_numbers(file_path):
    if not os.path.isfile(file_path):
        sys.exit(f"File {file_path} not found!")
        return []
    with open(file_path,'r') as summands:
        res = []
        for line in summands:
            # strip the newline character from the line
            line = line.strip()
            x = ast.literal_eval(line)
            res.append(x)
    return res

def inc_left_neighbor(num_list, indices, val):
    ind_test = indices.copy()
    while not ind_test == []:
        if ind_test[-1] != 0: # only interested if the last index is 1
            ind_test[-1] -= 1
        else:
            del ind_test[-1]
            continue
        list_cur = num_list
        for i in ind_test[:-1]:
            list_cur = list_cur[i]
        if isinstance(list_cur[ind_test[-1]], int):
            list_cur[ind_test[-1]] += val
            return num_list
        else: # left neighbor is a list -> go down this tree and find rightmost node in right subtree
            return inc_rightmost_int(list_cur[ind_test[-1]], val)
    return num_list

def inc_right_neighbor(num_list, indices, val):
    ind_test = indices.copy()
    while not ind_test == []:
        if ind_test[-1] == 0: # only interested if the last index is 0
            ind_test[-1] += 1
        else:
            del ind_test[-1]
            continue
        list_cur = num_list
        for i in ind_test[:-1]:
            list_cur = list_cur[i]
        if isinstance(list_cur[ind_test[-1]], int):
            list_cur[ind_test[-1]] += val
            return num_list
        else: # right neighbor is a list -> go down this tree and find leftmost node in right subtree
            return inc_leftmost_int(list_cur[ind_test[-1]], val)
    return num_list


def inc_leftmost_int(tree_list, val):
    if isinstance(tree_list[0], int):
        tree_list[0] += val
        return tree_list
    else:
        return inc_leftmost_int(tree_list[0], val)

def inc_rightmost_int(tree_list, val):
    if isinstance(tree_list[1], int):
        tree_list[1] += val
        return tree_list
    else:
        return inc_rightmost_int(tree_list[1], val)


def replace_with_value(num, indices, val):
    for i in indices[:-1]:
        num = num[i]
    if isinstance(num[0], list):
        num[0] = 0
    else:
        num[1] = 0

def reduce(num):
    pass

def explode(num):
    tmp = num.copy()
    res = {}
    res['Explode'] = True
    split_num = True
    while res['Explode'] or split_num:
        while res['Explode']:
            ind = []
            res = depth_check(tmp, ind)
            if res['Explode']:
                inc_left_neighbor(tmp, res['Indices'], res['Pair'][0])
                inc_right_neighbor(tmp, res['Indices'], res['Pair'][1])
                replace_with_value(tmp, res['Indices'], 0)
                split_num = True
        split_num = split(tmp)
        if split_num:
            res['Explode'] = True
    return tmp

# return list of indices to pair on level 4 or 0 otherwise
def depth_check(num, indices):
    if isinstance(num, list):
        if isinstance(num[0], list):
            left_idx = indices.copy()
            left_idx.append(0)
            if len(left_idx) > 3:
                return {'Explode': True, 'Pair': num[left_idx[-1]], 'Indices': left_idx} 
            else:
                res = depth_check(num[left_idx[-1]], left_idx)
                if (res['Explode']):
                    return res
        if isinstance(num[1], list):
            right_idx = indices.copy()
            right_idx.append(1)
            if len(right_idx) > 3:
                return {'Explode': True, 'Pair': num[right_idx[-1]], 'Indices': right_idx} 
            else:
                res = depth_check(num[right_idx[-1]], right_idx)
                if (res['Explode']):
                    return res
                else:
                    return {'Explode': False, 'Pair': [], 'Indices': []}
        else:
            return {'Explode': False, 'Pair': [], 'Indices': []}
        
    elif isinstance(num, int):
        return {'Explode': False, 'Pair': [], 'Indices': []}
    else:
        print(f"Failed to execute depthcheck for {num}")



def split(num):
    if isinstance(num, list):
        if isinstance(num[0], list):
            if split(num[0]):
                return True
        elif isinstance(num[0], int) and num[0] > 9:
            num[0] = [int(num[0]/2), int(num[0]/2+(num[0]%2))]
            return True
        if isinstance(num[1], list):
            if split(num[1]):
                return True
        elif isinstance(num[1], int) and num[1] > 9:
            num[1] = [int(num[1]/2), int(num[1]/2+(num[1]%2))]
            return True
    if isinstance(num, int) and num > 9:
        num = [int(num/2), int(num/2+(num%2))]
        return True
    return False

def add(num1, num2):
    return [num1, num2]

def magnitude(num):
    if isinstance(num, int):
        return num
    else:
        return 3 * magnitude(num[0]) + 2 * magnitude(num[1])


if __name__ == '__main__':
    # Read in snailfish numbers
    summands = read_numbers("/home/christoph/Dokumente/VSC/C++/AoC_2021/18/snailfish_summands.txt")
    summands = read_numbers(sys.argv[1])
    if not summands:
        sys.exit("Failed to read summands from file!")
    task2 = copy.deepcopy(summands)
    # First task
    sum = summands[0]
    for summand in summands[1:]:
        sum = add(sum, summand)
        explode(sum)
    print(f"Magnitude of total sum: {magnitude(sum)}")

    # Second task
    max_mag = 0
    for i in range(len(task2)):
        for j in range(len(task2)):
            if i != j:
                # continue
                s1 = copy.deepcopy(task2[i])
                s2 = copy.deepcopy(task2[j])
                sum = add(s1, s2)
                sum2 = list(sum[:])
                explode(sum2)
                mag = magnitude(sum)
                if mag > max_mag:
                    max_mag = mag
    # print(summands)
    print(f"Max magnitude of addition of two different snailfish numbers: {max_mag}")
