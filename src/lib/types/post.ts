export default interface Post {
	title: string;
	slug: string;
	date: string;
	excerpt: string;
	categories?: string | string[];
}
