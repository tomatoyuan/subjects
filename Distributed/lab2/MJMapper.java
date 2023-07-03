package task2;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.net.URI;
import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

/*
 * 1. 缓存 movies.dat （MovieID, MovieName, Genres电影类型） -> (MovieID, Genres)
 * 2. 读取 ratings.dat （UserID, MovieID, Rating, Timestamp）
 * 3. 进行 join 将读取的 ratings.dat 中的 电影评分变成一条评论记录IntWritable(1)，因为最后输出（MovieID, Genres, 评分次数）
 */

public class MJMapper {

	public static class StepOneMapper extends Mapper<Object, Text, Text, IntWritable>{
		/*
		 * 1. 缓存 movies.dat （MovieID, MovieName, Genres电影类型） -> (MovieID, Genres) setup
		 * 在任务开始的时候执行一次，map方法执行前
		 * 
		 * 作用：初始化
		 */
	
		private Map<String, String> movieMap = new HashMap<>();
	
		@Override
		protected void setup(Context context) throws IOException, InterruptedException {
			// 建立小表的 Hash 映射
			// 创建文件系统对象——目的创建流
	
			/*
			 * //使用的时候getLocalCacheArchives()这个方法被标记位过时，但是不影响使用 //获取缓存文件的路径 Path[]
			 * cacheFilePath = context.getLocalCacheArchives(); //由于只缓存了一个文件，所以在这里就不进行判断了
			 * String path = cacheFilePath[0].toUri().toString(); //通过File IO读取指定路径的文件进行处理
			 * BufferedReader bf1 = new BufferedReader(new FileReader(new File(path)));
			 * 
			 * //以上这些是为了在集群中运行
			 */
	
			// 在本地测试，这样读取缓存文件，因为文件已经缓存到工作空间中了，所以直接通过文件名读取即可
			BufferedReader bf = new BufferedReader(new FileReader(new File("movies.dat")));
			String line = null;
			while ((line = bf.readLine()) != null) {
				String[] movies = line.toString().split("::");
				// 这里直接将userId看作字符串进行处理
				String movieId = movies[0];
				String movieGenres = movies[2];
				movieMap.put(movieId, movieGenres);
			}
	
			// 关闭流
			bf.close();
	
		}
	
		/*
		 * 2. 读取 ratings.dat (UserID, MovieID, Rating, Timestamp)
		 * 
		 * 3. 进行 join 将读取的 ratings.dat 中的 电影评分替换电影名称，因为最后输出(MovieID, Genres, 评分次数)
		 */
		Text K = new Text();
		IntWritable V = new IntWritable(1);
		
		@Override
		protected void map(Object key, Text value, Context context) throws IOException, InterruptedException {
			// UserID, MovieID, Rating, Timestamp
			// 1::1193::5::978300760
			// 逐行读取ratings.dat文件 这里不多说读取的机制，详情见以后的源码分析
			String[] ratings = value.toString().split("::");
			String movieId = ratings[1];
	
			// 判断评分记录中的电影ID是否在电影信息中
			// 只要查到一条记录，就增加一条（评论）
			if (movieMap.containsKey(movieId)) {
				String outKey = movieId + "\t" + movieMap.get(movieId);
				K.set(outKey);
				context.write(K, V);
			}
		}

	}
	

	public static class StepOneReducer extends Reducer<Text, IntWritable, Text, IntWritable>{
	
		Text K = new Text();
		private IntWritable result = new IntWritable();
		
		@Override
		protected void reduce(Text key, Iterable<IntWritable> values, Context context)
				throws IOException, InterruptedException {
			
			//map端的输出最终可以看作 <k,{v1,v2,v3,...vn}>这种形式
			//所以在这里迭代统计评分次数，就是一个wordCount
			int sum = 0;
			for (IntWritable val : values) {
				//因为intWritable.get()都是1，所以直接++或者 += val
				sum += val.get();
			}
			result.set(sum);
			
			K.set(key.toString());
			context.write(K, result);
		}
	}
	
	public static void main(String[] args) throws Exception {
		
		/*
		 * step1: 合并两表，统计(MovieID, Genres, 评分次数)
		 */
		
		Configuration conf = new Configuration();
		FileSystem fs = FileSystem.get(conf);
		
		Job job = Job.getInstance(conf);
		
		job.setJarByClass(MJMapper.class);
	
//		 job.addArchiveToClassPath(new Path("")); //缓存压缩文件到classPath
//		 job.addCacheArchive(new URI("file:/E:/movies.dat")); //缓存普通文件到工作目录
//		 job.addFileToClassPath(new URI("file:/E:/movies.dat")); //缓存普通文件到classPath
		// 这里使用的时候缓存普通文件到工作目录
		// 这里要在本地进行测试，所以使用的本地文件系统的文件路径
		// 如果要打jar包运行在集群上，使用HDFS文件系统的路径
//		job.addCacheFile(new URI("file:///E:/movies.dat"));
		job.addCacheFile(new URI("/user/task2/movies.dat"));
	
		job.setMapperClass(StepOneMapper.class);
		job.setReducerClass(StepOneReducer.class);
	
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(IntWritable.class);
	
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(IntWritable.class);
	
		// 因为输入文件夹中有多个文件，为了区分，这里指定到文件名称
//		Path inputPath = new Path("E:\\ratings.dat");
		Path inputPath = new Path("/user/task2/ratings.dat");
//		Path outputPath = new Path("E:\\output1_1");
		Path outputPath = new Path("/user/task2/output");

		// 判断输出目录是否存在，如果存在，就删除
		if(fs.exists(outputPath))
		{
			fs.delete(outputPath, true);
		}
	
		// 设置数据的输入输出路径
		FileInputFormat.setInputPaths(job, inputPath);
		FileOutputFormat.setOutputPath(job, outputPath);
		
		/*
		 * step2: 对 MovieID 排序
		 * 			省略
		 */
		
		
		System.exit(job.waitForCompletion(true) ? 0 : 1);
	}

}
