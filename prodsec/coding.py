# https://leetcode.com/problems/zigzag-conversion/

""" v1: Initial solution """
class Solution(object):
    def convert(self, s, numRows):
        """
        :type s: str
        :type numRows: int
        :rtype: str
        """
        # init the output array
        numCols = len(s) / numRows + 1
        output =  [[' ' for _ in range(numCols)] for _ in range(numRows)]
        curCol, curRow = 0, 0
        down = True
        for i in range(len(s)):
            output[curRow][curCol] = s[i]
            if down:
                if curRow == numRows - 1: # was going down but now turn around
                    down = False
                    curCol += 1
                    curRow -= 1
                else:
                    curRow += 1
            else:
                if curRow == 0: # was going up but now turn around
                    down = True
                    curRow += 1
                else:
                    curRow -= 1
                    curCol += 1
        
        # iterate through output matrix to get the return value
        output_str = ""
        for r in range(numRows):
            for c in range(numCols):
                if output[r][c] != ' ':
                    output_str.append(output[r][c])
                c += 1
            r += 1
        return output_str

""" v2: More optimized solution """
class SolutionOptimized(object):
    def convert(self, s, numRows):
        if numRows == 1:
            return s
        arrs = [[] for _ in range(numRows)]
        cur_arr_index = 0
        down = True
        for i in range(s.length):
            arrs[cur_arr_index].append(s[i])
            cur_arr_index += 1 if down else -1 
            if cur_arr_index == 0 or cur_arr_index == numRows - 1:
                down = not down 
        return ''.join([''.join(arr) for arr in arrs])
