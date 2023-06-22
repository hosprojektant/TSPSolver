def NearestNeighbor(root, list_places, aco_len):
    # save all places cords
    counter = 0
    distance_arr = []
    actual_place = list_places[0]
    visited_places = []
    size = len(list_places)
    while True:
        counter += 1
        visited_places.append(actual_place.id)
        min = 9999
        min_id = 9999
        for id in range(len(actual_place.distance)):
            if id not in visited_places:
                if actual_place.distance[id] < min:  # get min
                    min = actual_place.distance[id]
                    min_id = id

        if counter == aco_len:
            break
        if min == 9999:  # check if we didnt visited all places
            break
        distance_arr.append(min)
        actual_place = list_places[min_id]

    return visited_places, sum(distance_arr)

# https://stackoverflow.com/questions/43588711/numpys-partition-slower-than-sort-for-small-arrays
# min = np.partition(v.distance, 1)[1] ## getting 2nd min value cuz first is self O(n) running time
# https://numpy.org/doc/stable/reference/generated/numpy.argpartition.html
# min_index = np.argpartition(v.distance, 1)[1]
