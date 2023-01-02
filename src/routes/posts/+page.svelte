<script lang="ts">
	import PostPreview from '$lib/components/PostPreview.svelte';
import type { PageData } from './$types';

	export let data: PageData;
</script>

<h1>Posts</h1>
      
<ul>
  {#each data.posts as post}
    <li>
        <PostPreview {post}></PostPreview>
    </li>
  {/each}
</ul>